#include "gemmini.h"

#include <bits/stdc++.h>
#include <ilang/ilang++.h>

namespace gemmini {

Gemmini::Gemmini(cfg Cfg)
    : m(Ila("Gemmini"))
    ,
    // ---------- Inputs ----------
    funct(m.NewBvInput("funct", 3))
    , rs1(m.NewBvInput("rs1", RS_WIDTH))
    , rs2(m.NewBvInput("rs2", RS_WIDTH))
    ,
    // ---------- Config states ----------
    dataflow(m.NewBoolState("dataflow"))
    , shift(m.NewBvState("shift", 32))
    , A_stride(m.NewBvState("A_stride", 16))
    , scale(m.NewBvState("scale", 32))
    , private_stride(m.NewBvState("private_stride", 16))
    , memory_stride_mvin(m.NewBvState("memory_stride_mvin", 64))
    , memory_stride_mvout(m.NewBvState("memory_stride_mvout", 64))
    , right_shift(m.NewBvState("right_shift", 32))
    , activation_func(m.NewBvState("activation_func", 1))
    , A_T(m.NewBvState("A_T", 1))
    , B_T(m.NewBvState("B_T", 1))
    , scalar(m.NewBvState("scalar", 32))
    , acc_type(m.NewBvState("acc_type", 1))
    , mvin_type(m.NewBvState("mvin_type", 2))
    , max_pool_stride(m.NewBvState("max_pool_stride", 2))
    , max_pool_window_size(m.NewBvState("max_pool_window_size", 2))
    , upper_zero_pad(m.NewBvState("upper_zero_pad", 2))
    , left_zero_pad(m.NewBvState("left_zero_pad", 2))
    , out_dim(m.NewBvState("out_dim", 8))
    , pool_row(m.NewBvState("pool_row", 8))
    , pool_col(m.NewBvState("pool_col", 8))
    , unpool_row(m.NewBvState("unpool_row", 8))
    , unpool_col(m.NewBvState("unpool_col", 8))
    ,
    // ---------- Extra for mvin and mvout----
    mvin_DRAM_addr(m.NewBvState("mvin_DRAM_addr", 64))
    , mvin_dest_addr(m.NewBvState("mvin_dest_addr", 32))
    , mvin_col_num(m.NewBvState("mvin_col_num", 16))
    , mvin_row_num(m.NewBvState("mvin_row_num", 16))
    , mvin_destination(m.NewBvState("mvin_destination", 1))
    , mvout_DRAM_addr(m.NewBvState("mvout_DRAM_addr", 64))
    , mvout_sour_addr(m.NewBvState("mvout_sour_addr", 32))
    , mvout_col_num(m.NewBvState("mvout_col_num", 16))
    , mvout_row_num(m.NewBvState("mvout_row_num", 16))
    , mvout_source(m.NewBvState("mvout_source", 1))
    , start_row(m.NewBvState("start_row", 16))
    , start_chunk(m.NewBvState("start_chunk", 16))
    , done(m.NewBoolState("done"))
    ,
    // ---------- Extra for preload and computation ----
    dest_addr(m.NewBvState("dest_addr", 32))
    , dest_row(m.NewBvState("dest_row", 16))
    , dest_col(m.NewBvState("dest_col", 16))
    , A_addr(m.NewBvState("A_addr", 32))
    , A_row(m.NewBvState("A_row", 16))
    , A_col(m.NewBvState("A_col", 16))
    , B_D_addr(m.NewBvState("B_D_addr", 32))
    , B_D_row(m.NewBvState("B_D_row", 16))
    , B_D_col(m.NewBvState("B_D_col", 16))
    , cycle(m.NewBvState("cycle", 32))
    , // arbitrary bit size?
    // ---------- Memory states --------------
    DRAM(m.NewMemState("DRAM", DRAM_ADDR_WIDTH, DRAM_DATA_WIDTH))
    , scratchpad(m.NewMemState("scratchpad", 32, Cfg.DIM * getBitWidth(Cfg.inputType)))
    , accumulator(m.NewMemState("accumulator", 32, Cfg.DIM * getBitWidth(Cfg.accType)))
{
    // ---------- Store config ----------
    _Cfg = Cfg;
    auto DIM = Cfg.DIM;
    // ---------- Create Systolic Array ----------
    sys_array.resize(DIM);
    for (size_t i = 0; i < DIM; i++) {
        sys_array[i].resize(DIM);
        for (size_t j = 0; j < DIM; j++) {
            sys_array[i][j] = std::make_unique<PE>(m, i, j);
        }
    }
};

void Gemmini::AddInstructions()
{
    const size_t DIM = _Cfg.DIM;
    const size_t INPUT_BITS = getBitWidth(_Cfg.inputType);
    const size_t ACC_BITS = getBitWidth(_Cfg.accType);
    const size_t OUTPUT_BITS = getBitWidth(_Cfg.outputType);
    const size_t INPUT_ROW_BITS = DIM * INPUT_BITS;
    const size_t ACC_ROW_BITS = DIM * ACC_BITS;

    {

        // Data Movement
        // rs1 is DRAM address
        {
            // mvin

            // mvin start
            {
                InstrRef instr = m.NewInstr("mvin");
                auto decode = mvin;
                instr.SetDecode(funct == decode);
                ExprRef DRAM_addr = Extract(rs1, 63, 0);
                ExprRef dest_addr = Extract(rs2, 31, 0);
                ExprRef col_num = Extract(rs2, 47, 32);
                ExprRef row_num = Extract(rs2, 63, 48);
                ExprRef destination = Extract(rs2, 31, 31);

                instr.SetUpdate(start_row, BvConst(0, 16));
                instr.SetUpdate(start_chunk, BvConst(0, 16));
                instr.SetUpdate(done, BoolConst(false));
                instr.SetUpdate(mvin_DRAM_addr, DRAM_addr);
                instr.SetUpdate(mvin_dest_addr, dest_addr);
                instr.SetUpdate(mvin_row_num, row_num);
                instr.SetUpdate(mvin_col_num, col_num);
                instr.SetUpdate(mvin_destination, destination);
            }

            // mvin step
            {
                InstrRef instr = m.NewInstr("mvin_step");
                instr.SetDecode((funct == mvin) & !done);

                // Total chunks per row
                auto total_chunks = (mvin_col_num + BvConst(DIM - 1, 16)) / BvConst(DIM, 16);

                // Determine how many columns to process per chunk
                auto col_start = start_chunk * BvConst(DIM, 16);
                auto chunk_end = Ite((col_start + BvConst(DIM, 16)) <= mvin_col_num,
                    col_start + BvConst(DIM, 16),
                    mvin_col_num);
                auto chunk_width = chunk_end - col_start;

                auto chunk_valid = Ite(Ult(col_start, mvin_col_num), SYMB_TRUE, SYMB_FALSE);
                auto should_transfer = Ite(!done & chunk_valid, SYMB_TRUE, SYMB_FALSE);

                auto dram_base = mvin_DRAM_addr + (ZExt(start_row, DRAM_ADDR_WIDTH) * memory_stride_mvin) + (ZExt(start_chunk, DRAM_ADDR_WIDTH) * BvConst(DIM, DRAM_ADDR_WIDTH));
                auto dest_base = mvin_dest_addr + (ZExt(start_row, 32)) + (ZExt(start_chunk, 32) * ZExt(private_stride, 32));

                auto current_row_input = scratchpad.Load(dest_base);
                auto current_row_acc = accumulator.Load(dest_base);
                ExprRef new_row_input_concat = BvConst(0, INPUT_ROW_BITS);
                ExprRef new_row_acc_concat = BvConst(0, ACC_ROW_BITS);

                // Build the row
                for (size_t elem = 0; elem < DIM; elem++) {
                    auto elem_valid = Ite(BvConst(elem, 16) < chunk_width, SYMB_TRUE, SYMB_FALSE);
                    auto should_update = Ite(should_transfer & elem_valid, SYMB_TRUE, SYMB_FALSE);

                    auto dram_elem_addr = dram_base + BvConst(elem, DRAM_ADDR_WIDTH); 
                    auto load_elem = DRAM.Load(dram_elem_addr);

                    auto load_elem_input = Extract(load_elem, INPUT_BITS - 1, 0);
                    auto load_elem_acc = Extract(load_elem, ACC_BITS - 1, 0);

                    if (elem == 0) {
                        new_row_input_concat = load_elem_input;
                        new_row_acc_concat = load_elem_acc;
                    } else {
                        auto new_elem_input = Concat(load_elem_input, new_row_input_concat);
                        auto new_elem_acc = Concat(load_elem_acc, new_row_acc_concat);

                        auto old_padded_input = ZExt(new_row_input_concat, INPUT_BITS + (elem * INPUT_BITS));
                        auto old_padded_acc = ZExt(new_row_acc_concat, ACC_BITS + (elem * ACC_BITS));

                        new_row_input_concat = Ite(should_update, new_elem_input, old_padded_input);
                        new_row_acc_concat = Ite(should_update, new_elem_acc, old_padded_acc);
                    }
                }

                // Decide whether to update or not
                auto store_sp = scratchpad.Store(dest_base, new_row_input_concat);
                auto store_acc_input = accumulator.Store(dest_base, ZExt(new_row_input_concat, ACC_ROW_BITS));
                auto store_acc = accumulator.Store(dest_base, new_row_acc_concat);
                instr.SetUpdate(scratchpad, Ite(should_transfer & (mvin_destination == BvConst(0, 1)), store_sp, scratchpad));
                instr.SetUpdate(accumulator, Ite(should_transfer & (mvin_destination == BvConst(1, 1)), Ite(acc_type == BvConst(0, 1), store_acc, store_acc_input), accumulator));

                // Advance chunk counter
                auto next_chunk = start_chunk + BvConst(1, 16);
                auto chunk_overflow = Ite(Uge(next_chunk, total_chunks), SYMB_TRUE, SYMB_FALSE);
                auto new_chunk = Ite(chunk_overflow, BvConst(0, 16), next_chunk);
                auto new_row = Ite(chunk_overflow, start_row + BvConst(1, 16), start_row);

                instr.SetUpdate(start_chunk, Ite(!done, new_chunk, start_chunk));
                instr.SetUpdate(start_row, Ite(!done, new_row, start_row));

                // Set done when all chunks and rows are done
                auto all_rows_done = Uge(new_row, mvin_row_num);
                auto done_after = Ite(chunk_overflow & all_rows_done, SYMB_TRUE, SYMB_FALSE);
                instr.SetUpdate(done, Ite(done_after, BoolConst(true), done));
            }

        }

        {
            // mvout start
            {
                InstrRef instr = m.NewInstr("mvout");
                auto decode = mvout;
                instr.SetDecode(funct == decode);
                auto DRAM_addr = Extract(rs1, 63, 0);
                auto sour_addr = Extract(rs2, 31, 0);
                auto col_num = Extract(rs2, 47, 32);
                auto row_num = Extract(rs2, 63, 48);
                auto source = Extract(rs2, 31, 31);

                instr.SetUpdate(start_row, BvConst(0, 16));
                instr.SetUpdate(start_chunk, BvConst(0, 16));
                instr.SetUpdate(done, BoolConst(false));
                instr.SetUpdate(mvout_DRAM_addr, DRAM_addr);
                instr.SetUpdate(mvout_sour_addr, sour_addr);
                instr.SetUpdate(mvout_row_num, row_num);
                instr.SetUpdate(mvout_col_num, col_num);
                instr.SetUpdate(mvout_source, source);
            }

            // mvout step
            {
                InstrRef instr = m.NewInstr("mvout_step");
                auto decode = mvout;
                instr.SetDecode((funct == decode) & !done);

                // Check if need to continue or not
                auto continue_row = Ite(start_row < mvout_row_num, SYMB_TRUE, SYMB_FALSE);

                // Get base address for each row
                auto dram_base = mvout_DRAM_addr + (ZExt(start_row, DRAM_ADDR_WIDTH) * memory_stride_mvout);
                auto sour_base = mvout_sour_addr + (ZExt(start_row, 32));

                // Get the row to be transferred
                auto current_row_input = scratchpad.Load(sour_base);
                auto current_row_acc = accumulator.Load(sour_base);

                ExprRef dram_next = DRAM;

                for (size_t elem = 0; elem < DIM; elem++) {
                    auto dram_elem_addr = dram_base + BvConst(elem, DRAM_ADDR_WIDTH);

                    auto elem_input = Extract(current_row_input, (elem + 1) * INPUT_BITS - 1, elem * INPUT_BITS);
                    auto pre_scaled_acc = Extract(current_row_acc, (elem + 1) * ACC_BITS - 1, elem * ACC_BITS);
                    auto elem_acc = ScaleBv(pre_scaled_acc, scalar, INPUT_BITS);

                    auto update = Ite(BvConst(elem, 16) < mvout_col_num, SYMB_TRUE, SYMB_FALSE);

                    auto select_source = Ite(mvout_source == BvConst(0, 1),
                        ResizeBv(elem_input, DRAM_DATA_WIDTH),
                        ResizeBv(elem_acc, DRAM_DATA_WIDTH));

                    dram_next = Ite(update & continue_row,
                        Store(dram_next, dram_elem_addr, select_source),
                        dram_next);
                }

                instr.SetUpdate(DRAM, dram_next);

                // Go to next row
                instr.SetUpdate(start_row, start_row + BvConst(1, 16));

                // If rows are all processed, stop
                instr.SetUpdate(done, Ite(!continue_row, BoolConst(true), done));
            }
        }
    }

    {
        // Configuration
        {
            // config_ex
            InstrRef instr = m.NewInstr("config_ex");
            auto decode = config;
            auto type = Extract(rs1, 1, 0);
            instr.SetDecode((funct == decode) & (type == BvConst(0, 2)));
            instr.SetUpdate(dataflow, Ite(Extract(rs1, 2, 2) == BvConst(1, 1), BoolConst(true), BoolConst(false)));
            instr.SetUpdate(activation_func, Extract(rs1, 3, 3));
            instr.SetUpdate(A_T, Extract(rs1, 8, 8));
            instr.SetUpdate(B_T, Extract(rs1, 9, 9));
            instr.SetUpdate(A_stride, Extract(rs1, 31, 16));
            instr.SetUpdate(scalar, Extract(rs1, 63, 32));
            instr.SetUpdate(right_shift, Extract(rs2, 31, 0));
        }

        {
            // config_mvin
            InstrRef instr = m.NewInstr("config_mvin");
            auto decode = config;
            auto type = Extract(rs1, 1, 0);
            instr.SetDecode((funct == decode) & (type == BvConst(1, 2)));
            instr.SetUpdate(acc_type, Extract(rs1, 2, 2));
            instr.SetUpdate(mvin_type, Extract(rs1, 4, 3));
            instr.SetUpdate(private_stride, Extract(rs1, 31, 16));
            instr.SetUpdate(memory_stride_mvin, Extract(rs2, 63, 0));
            instr.SetUpdate(scale, Extract(rs1, 63, 32));
        }

        {
            // config_mvout
            InstrRef instr = m.NewInstr("config_mvout");
            auto decode = config;
            auto type = Extract(rs1, 1, 0);
            instr.SetDecode((funct == decode) & (type == BvConst(2, 2)));
            instr.SetUpdate(max_pool_stride, Extract(rs1, 5, 4));
            instr.SetUpdate(max_pool_window_size, Extract(rs1, 7, 6));
            instr.SetUpdate(upper_zero_pad, Extract(rs1, 9, 8));
            instr.SetUpdate(left_zero_pad, Extract(rs1, 11, 10));
            instr.SetUpdate(out_dim, Extract(rs1, 31, 24));
            instr.SetUpdate(pool_row, Extract(rs1, 39, 32));
            instr.SetUpdate(pool_col, Extract(rs1, 47, 40));
            instr.SetUpdate(unpool_row, Extract(rs1, 55, 48));
            instr.SetUpdate(unpool_col, Extract(rs1, 63, 56));
            instr.SetUpdate(memory_stride_mvout, Extract(rs2, 63, 0));
        }
    }

    {
        // Core matmul sequence
        {
            // matmul.preload
            InstrRef instr = m.NewInstr("matmul.preload");
            auto decode = matmul_preload;
            instr.SetDecode(funct == decode);
            auto source_addr = Extract(rs1, 31, 0);
            auto source_col = Extract(rs1, 47, 32);
            auto source_row = Extract(rs1, 63, 48);
            instr.SetUpdate(dest_addr, Extract(rs2, 31, 0));
            instr.SetUpdate(dest_col, Extract(rs2, 47, 32));
            instr.SetUpdate(dest_row, Extract(rs2, 63, 48));

            // FIX: this instruction loads D in OS mode (never transposed) and B in
            // WS mode (transposed iff B_T is set). Gate transpose on WS mode so an
            // OS-mode preload of D can never be accidentally transposed.
            auto os_mode = (dataflow == BoolConst(false));
            auto B_transpose = (B_T == BvConst(1, 1));
            auto apply_transpose = !os_mode & B_transpose;

            for (size_t i = 0; i < DIM; i++) {
                auto& sys_row = sys_array[i];
                for (size_t j = 0; j < DIM; j++) {
                    // Non-transposed read: physical row i, column j (original behavior)
                    auto row_index_nt = BvConst(i, 32);
                    auto row_sp_nt = scratchpad.Load(source_addr + row_index_nt);
                    auto row_acc_nt = accumulator.Load(source_addr + row_index_nt);
                    auto elem_sp_nt = Extract(row_sp_nt, (j + 1) * INPUT_BITS - 1, j * INPUT_BITS);
                    auto elem_acc_nt = Extract(row_acc_nt, (j + 1) * ACC_BITS - 1, j * ACC_BITS);

                    // Transposed read: physical row j, column i, i.e. stationary[i][j] = physical(j,i)
                    auto row_index_t = BvConst(j, 32);
                    auto row_sp_t = scratchpad.Load(source_addr + row_index_t);
                    auto row_acc_t = accumulator.Load(source_addr + row_index_t);
                    auto elem_sp_t = Extract(row_sp_t, (i + 1) * INPUT_BITS - 1, i * INPUT_BITS);
                    auto elem_acc_t = Extract(row_acc_t, (i + 1) * ACC_BITS - 1, i * ACC_BITS);

                    auto elem_sp = Ite(apply_transpose, elem_sp_t, elem_sp_nt);
                    auto elem_acc = Ite(apply_transpose, elem_acc_t, elem_acc_nt);

                    auto preload_elem = Ite(Extract(source_addr, 31, 31) == BvConst(0, 1), ResizeBv(elem_sp, ACC_BITS), elem_acc);

                    // FIX: bounds must swap under transpose too (mirrors in_bounds fix
                    // in compute.preloaded_step) — the "row-count" and "col-count" of
                    // the *used* (post-transpose) matrix are swapped from the stored one.
                    auto row_bound = Ite(apply_transpose, source_col, source_row);
                    auto col_bound = Ite(apply_transpose, source_row, source_col);
                    auto should_transfer = Ite((BvConst(i, 16) < row_bound) & (BvConst(j, 16) < col_bound), SYMB_TRUE, SYMB_FALSE);
                    instr.SetUpdate(sys_row[j]->stationary_reg, Ite(should_transfer, preload_elem, sys_row[j]->stationary_reg));
                }
            }
        }

        {
            InstrRef instr = m.NewInstr("matmul.compute.atomic");
            auto decode = matmul_compute_atomic;
            instr.SetDecode(funct == decode);
            auto A_scratchpad_addr = Extract(rs1, 31, 0);
            auto A_col_ = Extract(rs1, 47, 32);
            auto A_row_ = Extract(rs1, 63, 48);
            auto BD_scratchpad_addr = Extract(rs2, 31, 0);
            auto BD_col = Extract(rs2, 47, 32);
            auto BD_row = Extract(rs2, 63, 48);

            auto os_mode = (dataflow == BoolConst(false));
            auto A_transpose = (A_T == BvConst(1, 1)); // ASSUMPTION: A_T exists as a sibling of B_T in config_ex
            auto B_transpose = (B_T == BvConst(1, 1));

            // --- Bounds, mirroring the row_bound/col_bound swap pattern from matmul.preload ---
            // A convention (same asymmetry as documented for A_1): A_T=1 -> natural row-major
            // (physical row=i, col=k); A_T=0 (default) -> pre-transposed (physical row=k, col=i).
            auto A_i_bound = Ite(A_transpose, A_row_, A_col_); // bounds output row i
            auto A_k_bound = Ite(A_transpose, A_col_, A_row_); // bounds contraction index k

            // B convention (OS mode direct read), same as matmul.preload's B_T handling:
            auto B_k_bound = Ite(B_transpose, BD_col, BD_row); // bounds contraction index k
            auto B_j_bound = Ite(B_transpose, BD_row, BD_col); // bounds output col j

            // D is never transposed (decoupled from transpose muxes) -> straight bounds.
            auto D_i_bound = BD_row;
            auto D_j_bound = BD_col;
            auto new_scratchpad = scratchpad;
            auto new_accumulator = accumulator;     

            for (size_t i = 0; i < DIM; i++) {
                auto& sys_row = sys_array[i];

                // Row buffers for the writeback, built up per-column below.
                std::vector<ExprRef> sp_row_elems(DIM, BvConst(0, INPUT_BITS));
                std::vector<ExprRef> acc_row_elems(DIM, BvConst(0, ACC_BITS));
                std::vector<ExprRef> row_write_valid(DIM, SYMB_FALSE);

                for (size_t j = 0; j < DIM; j++) {
                    // ---------- OS mode: A*B accumulated into stationary_reg (seeded by D from preload) ----------
                    ExprRef os_sum = BvConst(0, ACC_BITS);
                    for (size_t k = 0; k < DIM; k++) {
                        auto a_addr_row = Ite(A_transpose, BvConst(i, 32), BvConst(k, 32));
                        auto a_row_sp = scratchpad.Load(A_scratchpad_addr + a_addr_row);
                        auto a_row_acc = accumulator.Load(A_scratchpad_addr + a_addr_row);
                        auto a_col_off = Ite(A_transpose, BvConst(k, 32), BvConst(i, 32));
                        // Extract requires compile-time bit indices; since a_col_off is symbolic here in
                        // general you'd need a mux over the DIM possible extract positions. Since A_transpose
                        // only changes WHICH of {k,i} is used, and both are compile-time in this unrolled
                        // loop, build both extractions and Ite between them instead of an Extract-by-variable:
                        auto a_elem_default = Extract(a_row_sp, (i + 1) * INPUT_BITS - 1, i * INPUT_BITS); // A_T=0: col=i
                        auto a_elem_T = Extract(scratchpad.Load(A_scratchpad_addr + BvConst(i, 32)),
                            (k + 1) * INPUT_BITS - 1, k * INPUT_BITS); // A_T=1: row=i,col=k
                        auto a_elem_default_acc = Extract(a_row_acc, (i + 1) * ACC_BITS - 1, i * ACC_BITS);
                        auto a_elem_T_acc = Extract(accumulator.Load(A_scratchpad_addr + BvConst(i, 32)),
                            (k + 1) * ACC_BITS - 1, k * ACC_BITS);

                        auto a_from_sp = Ite(A_transpose, ResizeBv(a_elem_T, ACC_BITS), ResizeBv(a_elem_default, ACC_BITS));
                        auto a_from_acc = Ite(A_transpose, a_elem_T_acc, a_elem_default_acc);
                        auto a_elem = Ite(Extract(A_scratchpad_addr, 31, 31) == BvConst(0, 1), a_from_sp, a_from_acc);

                        // B element, OS mode: read directly from BD address, same swap convention as preload's B_T
                        auto b_addr_row_nt = BvConst(k, 32);
                        auto b_addr_row_t = BvConst(j, 32);
                        auto b_row_sp = scratchpad.Load(BD_scratchpad_addr + Ite(B_transpose, b_addr_row_t, b_addr_row_nt));
                        auto b_row_acc = accumulator.Load(BD_scratchpad_addr + Ite(B_transpose, b_addr_row_t, b_addr_row_nt));
                        auto b_elem_sp_nt = Extract(scratchpad.Load(BD_scratchpad_addr + BvConst(k, 32)),
                            (j + 1) * INPUT_BITS - 1, j * INPUT_BITS);
                        auto b_elem_sp_t = Extract(scratchpad.Load(BD_scratchpad_addr + BvConst(j, 32)),
                            (k + 1) * INPUT_BITS - 1, k * INPUT_BITS);
                        auto b_elem_acc_nt = Extract(accumulator.Load(BD_scratchpad_addr + BvConst(k, 32)),
                            (j + 1) * ACC_BITS - 1, j * ACC_BITS);
                        auto b_elem_acc_t = Extract(accumulator.Load(BD_scratchpad_addr + BvConst(j, 32)),
                            (k + 1) * ACC_BITS - 1, k * ACC_BITS);
                        auto b_from_sp = Ite(B_transpose, ResizeBv(b_elem_sp_t, ACC_BITS), ResizeBv(b_elem_sp_nt, ACC_BITS));
                        auto b_from_acc = Ite(B_transpose, b_elem_acc_t, b_elem_acc_nt);
                        auto b_elem = Ite(Extract(BD_scratchpad_addr, 31, 31) == BvConst(0, 1), b_from_sp, b_from_acc);

                        auto k_valid = Ite((BvConst(k, 16) < A_k_bound) & (BvConst(k, 16) < B_k_bound),
                            SYMB_TRUE, SYMB_FALSE);
                        auto product = a_elem * b_elem; // both already ACC_BITS-wide
                        os_sum = os_sum + Ite(k_valid, product, BvConst(0, ACC_BITS));
                    }
                    auto new_val_os = sys_row[j]->stationary_reg + os_sum; // seed (D) was preloaded here already
                    auto out_valid_os = Ite((BvConst(i, 16) < A_i_bound) & (BvConst(j, 16) < B_j_bound),
                        SYMB_TRUE, SYMB_FALSE);

                    // ---------- WS mode: A * stationary_reg (weights), D streamed fresh from BD address ----------
                    ExprRef ws_sum = BvConst(0, ACC_BITS);
                    for (size_t k = 0; k < DIM; k++) {
                        auto a_elem_default = Extract(scratchpad.Load(A_scratchpad_addr + BvConst(i, 32)), (k + 1) * INPUT_BITS - 1, k * INPUT_BITS);
                        auto a_elem_T = Extract(scratchpad.Load(A_scratchpad_addr + BvConst(k, 32)), (i + 1) * INPUT_BITS - 1, i * INPUT_BITS);
                        auto a_elem_default_acc = Extract(accumulator.Load(A_scratchpad_addr + BvConst(k, 32)),
                            (i + 1) * ACC_BITS - 1, i * ACC_BITS);
                        auto a_elem_T_acc = Extract(accumulator.Load(A_scratchpad_addr + BvConst(i, 32)),
                            (k + 1) * ACC_BITS - 1, k * ACC_BITS);
                        auto a_from_sp = Ite(A_transpose, ResizeBv(a_elem_T, ACC_BITS), ResizeBv(a_elem_default, ACC_BITS));
                        auto a_from_acc = Ite(A_transpose, a_elem_T_acc, a_elem_default_acc);
                        auto a_elem = Ite(Extract(A_scratchpad_addr, 31, 31) == BvConst(0, 1), a_from_sp, a_from_acc);

                        // Weight comes from the PE array itself (already loaded by matmul.preload),
                        // not from scratchpad. It was stored ACC_BITS-wide (resized), take the low
                        // INPUT_BITS back out for the multiply.
                        auto b_elem_weight = ResizeBv(Extract(sys_array[k][j]->stationary_reg, INPUT_BITS - 1, 0), ACC_BITS);

                        auto k_valid = Ite(BvConst(k, 16) < A_k_bound, SYMB_TRUE, SYMB_FALSE);
                        auto product = a_elem * b_elem_weight;
                        ws_sum = ws_sum + Ite(k_valid, product, BvConst(0, ACC_BITS));
                    }
                    // D seed, streamed via the shared BD port in WS mode — never transposed.
                    auto d_elem_sp = ResizeBv(Extract(scratchpad.Load(BD_scratchpad_addr + BvConst(i, 32)),
                                                  (j + 1) * INPUT_BITS - 1, j * INPUT_BITS),
                        ACC_BITS);
                    auto d_elem_acc = Extract(accumulator.Load(BD_scratchpad_addr + BvConst(i, 32)),
                        (j + 1) * ACC_BITS - 1, j * ACC_BITS);
                    auto d_elem = Ite(Extract(BD_scratchpad_addr, 31, 31) == BvConst(0, 1), d_elem_sp, d_elem_acc);

                    auto new_val_ws = d_elem + ws_sum;
                    auto out_valid_ws = Ite((BvConst(i, 16) < D_i_bound) & (BvConst(j, 16) < D_j_bound) & (BvConst(i, 16) < A_i_bound), SYMB_TRUE, SYMB_FALSE);

                    // ---------- Combine modes ----------
                    auto combined_val = Ite(os_mode, new_val_os, new_val_ws);
                    auto combined_valid = Ite(os_mode, out_valid_os, out_valid_ws);

                    // OS mode: result stays in the PE (stationary). WS mode: weight register is untouched.
                    instr.SetUpdate(sys_row[j]->stationary_reg,
                        Ite(os_mode & combined_valid, new_val_os, sys_row[j]->stationary_reg));

                    // ---------- Right shift (OS mode only) applied when narrowing to scratchpad width ----------
                    auto shift_amt = ResizeBv(right_shift, ACC_BITS); // ASSUMPTION: state var name for the OS shift amount
                    auto os_shifted = new_val_os >> shift_amt; // ASSUMPTION: arithmetic-right-shift helper name
                    auto elem_sp_new = Ite(os_mode, Extract(os_shifted, INPUT_BITS - 1, 0),
                        Extract(new_val_ws, INPUT_BITS - 1, 0)); // WS: plain truncate, no shift
                    auto elem_acc_new = combined_val; // full precision to accumulator, no shift in either mode

                    row_write_valid[j] = combined_valid;
                    sp_row_elems[j] = elem_sp_new;
                    acc_row_elems[j] = elem_acc_new;
                }

                // ---------- Writeback: one row, non-destructive per element, target chosen by dest_addr[31] ----------
                auto to_scratchpad = (Extract(dest_addr, 31, 31) == BvConst(0, 1));

                auto old_sp_row = scratchpad.Load(dest_addr + BvConst(i, 32));
                auto old_acc_row = accumulator.Load(dest_addr + BvConst(i, 32));

                ExprRef sp_row_val = BvConst(0, INPUT_ROW_BITS);
                ExprRef acc_row_val = BvConst(0, ACC_ROW_BITS);
                for (size_t j = 0; j < DIM; j++) {
                    auto old_sp_elem = Extract(old_sp_row, (j + 1) * INPUT_BITS - 1, j * INPUT_BITS);
                    auto old_acc_elem = Extract(old_acc_row, (j + 1) * ACC_BITS - 1, j * ACC_BITS);

                    auto write_sp = row_write_valid[j] & to_scratchpad;
                    auto write_acc = row_write_valid[j] & !to_scratchpad;

                    auto sp_elem = Ite(write_sp, sp_row_elems[j], old_sp_elem);
                    auto acc_elem = Ite(write_acc, acc_row_elems[j], old_acc_elem);

                    sp_row_val = (j == 0) ? sp_elem : Concat(sp_elem, sp_row_val); // ASSUMPTION: Concat(hi, lo)
                    acc_row_val = (j == 0) ? acc_elem : Concat(acc_elem, acc_row_val);
                }

                new_scratchpad  = new_scratchpad.Store(dest_addr + BvConst(i, 32), sp_row_val);
                new_accumulator = new_accumulator.Store(dest_addr + BvConst(i, 32), acc_row_val);
            }

            instr.SetUpdate(scratchpad, new_scratchpad);
            instr.SetUpdate(accumulator, new_accumulator);
        }

        {

            {
                // matmul.compute.preloaded
                InstrRef instr = m.NewInstr("matmul.compute.preloaded");
                auto decode = matmul_compute_preloaded;
                instr.SetDecode(funct == decode);
                auto A_scratchpad_addr = Extract(rs1, 31, 0);
                auto A_col_ = Extract(rs1, 47, 32);
                auto A_row_ = Extract(rs1, 63, 48);
                auto BD_scratchpad_addr = Extract(rs2, 31, 0);
                auto BD_col = Extract(rs2, 47, 32);
                auto BD_row = Extract(rs2, 63, 48);

                instr.SetUpdate(A_addr, A_scratchpad_addr);
                instr.SetUpdate(A_row, A_row_);
                instr.SetUpdate(A_col, A_col_);
                instr.SetUpdate(B_D_addr, BD_scratchpad_addr);
                instr.SetUpdate(B_D_row, BD_row);
                instr.SetUpdate(B_D_col, BD_col);
                instr.SetUpdate(cycle, BvConst(0, 32));
            }

            {

                // matmul.compute.preloaded step
                InstrRef instr = m.NewInstr("matmul.compute.preloaded_step");
                auto decode = matmul_compute_preloaded;
                instr.SetDecode(funct == decode & (cycle <= BvConst((2 * DIM) - 1, 32)));
                auto os_mode = (dataflow == BoolConst(false));
                auto write_cycle = os_mode & (cycle == BvConst((2 * DIM) - 1, 32));
                auto destination = Extract(dest_addr, 31, 31);

                ExprRef scratchpad_next2 = scratchpad;
                ExprRef accumulator_next2 = accumulator;

                auto A_transpose = A_T == BvConst(1, 1);
                auto B_transpose = B_T == BvConst(1, 1);
                auto ReLU = activation_func == BvConst(1, 1);

                // FIX: in_bounds must account for the active region of the *fed* operand,
                // which changes shape under transpose. A_transpose feeds A_col "rows";
                // B_transpose feeds B_D_row "columns".
                auto A_active_rows = Ite(A_transpose, ZExt(A_col, 16), A_row);
                auto B_active_cols = Ite(B_transpose, ZExt(B_D_row, 16), B_D_col);

                for (size_t row = 0; row < DIM; row++) {
                    auto is_last_row = Ite(BvConst(row + 1, 16) == dest_row, SYMB_TRUE, SYMB_FALSE);

                    for (size_t col = 0; col < DIM; col++) {
                        auto in_bounds = (BvConst(row, 16) < A_active_rows) & (BvConst(col, 16) < B_active_cols);

                        ExprRef A_in = BvConst(0, INPUT_BITS);
                        ExprRef B_D_in = BvConst(0, INPUT_BITS);

                        if (col == 0) {
                            auto k_a = cycle - BvConst(row, 32);
                            auto A_1 = Ite(cycle >= BvConst(row, 32) & k_a < ZExt(A_col, 32) & !write_cycle,
                                Extract(Lshr(scratchpad.Load(A_addr + (BvConst(row, GEMMINI_ADDR_WIDTH) * ResizeBv(A_stride, GEMMINI_ADDR_WIDTH))),
                                            ResizeBv(k_a * BvConst(INPUT_BITS, 32), INPUT_ROW_BITS)),
                                    INPUT_BITS - 1, 0),
                                BvConst(0, INPUT_BITS));
                            auto k_row = cycle - BvConst(row, 32);
                            auto A_2 = Ite(cycle >= BvConst(row, 32) & k_row < ZExt(A_row, 32) & !write_cycle,
                                Extract(scratchpad.Load(A_addr + ResizeBv(k_row, GEMMINI_ADDR_WIDTH) * ResizeBv(A_stride, GEMMINI_ADDR_WIDTH)),
                                    (row + 1) * INPUT_BITS - 1,
                                    row * INPUT_BITS),
                                BvConst(0, INPUT_BITS));

                            // FIX: "row" = output row (m) in OS, but = contraction index (k) in WS.
                            // A_1/A_2 therefore swap which one is "natural" vs "transposed" depending
                            // on mode. Keep OS's existing (already-correct) mapping; flip it for WS
                            // so A_T means the same thing (0=natural, 1=transposed) in both modes.
                            auto A_in_os = Ite(A_transpose, A_2, A_1);
                            auto A_in_ws = Ite(A_transpose, A_1, A_2);
                            A_in = Ite(os_mode, A_in_os, A_in_ws);
                        } else {
                            A_in = sys_array[row][col - 1]->A_reg;
                        }

                        if (row == 0) {
                            auto k_b = cycle - BvConst(col, 32);
                            auto B_1 = Ite(cycle >= BvConst(col, 32) & k_b < ZExt(B_D_row, 32) & !write_cycle,
                                Extract(scratchpad.Load(B_D_addr + k_b),
                                    (col + 1) * INPUT_BITS - 1,
                                    col * INPUT_BITS),
                                BvConst(0, INPUT_BITS));
                            // FIX: keep col-based skew and col-pitched address (mirror of A_2's fix);
                            // was incorrectly keyed off `row` (which is 0 in this scope).
                            auto k_col = cycle - BvConst(col, 32);
                            auto B_2 = Ite(cycle >= BvConst(col, 32) & k_col < ZExt(B_D_col, 32) & !write_cycle,
                                Extract(Lshr(scratchpad.Load(B_D_addr + BvConst(col, GEMMINI_ADDR_WIDTH)),
                                            ResizeBv(k_col * BvConst(INPUT_BITS, 32), INPUT_ROW_BITS)),
                                    INPUT_BITS - 1, 0),
                                BvConst(0, INPUT_BITS));
                            B_D_in = Ite(B_transpose, B_2, B_1);
                        } else {
                            B_D_in = sys_array[row - 1][col]->B_D_reg;
                        }

                        instr.SetUpdate(sys_array[row][col]->A_reg, Ite(in_bounds & !write_cycle, A_in, sys_array[row][col]->A_reg));
                        instr.SetUpdate(sys_array[row][col]->B_D_reg, Ite(in_bounds & !write_cycle, B_D_in, sys_array[row][col]->B_D_reg));

                        auto weight = Ite(os_mode, ZExt(B_D_in, ACC_BITS), sys_array[row][col]->stationary_reg);
                        auto product = ZExt(A_in, ACC_BITS) * weight;

                        auto stat_updated = sys_array[row][col]->stationary_reg + product;
                        instr.SetUpdate(sys_array[row][col]->stationary_reg,
                            Ite(os_mode & in_bounds & !write_cycle, stat_updated, sys_array[row][col]->stationary_reg));
                        auto c_os = Extract(stat_updated, OUTPUT_BITS - 1, 0);

                        // D (psum) path: intentionally independent of A_transpose/B_transpose.
                        // config_ex has no D-transpose option, so this must never route
                        // through the A_2/B_2 transpose muxes above.
                        ExprRef psum_in = BvConst(0, ACC_BITS);
                        if (row == 0) {
                            auto k_p = cycle - BvConst(col, 32);
                            psum_in = Ite(cycle >= BvConst(col, 32) & k_p < ZExt(B_D_row, 32),
                                ZExt(Extract(scratchpad.Load(B_D_addr + k_p),
                                         (col + 1) * INPUT_BITS - 1, col * INPUT_BITS),
                                    ACC_BITS),
                                BvConst(0, ACC_BITS));
                        } else {
                            psum_in = ZExt(sys_array[row - 1][col]->C_reg_out, ACC_BITS);
                        }
                        auto c_ws = Extract(product + psum_in, OUTPUT_BITS - 1, 0);

                        instr.SetUpdate(sys_array[row][col]->C_reg_out,
                            Ite(in_bounds & !write_cycle, Ite(os_mode, c_os, c_ws), sys_array[row][col]->C_reg_out));

                        auto row_off = BvConst(row, 32);
                        auto col_off = BvConst(col, 32);
                        auto instance = cycle - row_off - col_off;
                        auto col_valid = is_last_row
                            & (cycle >= row_off + col_off)
                            & (instance < ZExt(dest_row, 32)) // Could underflow?
                            & Ite((BvConst(col, 16) < dest_col), SYMB_TRUE, SYMB_FALSE);
                        auto writeAddrCol = dest_addr + instance;

                        auto existingRowSp = scratchpad_next2.Load(writeAddrCol);
                        auto existingRowAcc = accumulator_next2.Load(writeAddrCol);
                        ExprRef newRowSp = BvConst(0, INPUT_ROW_BITS);
                        ExprRef newRowAcc = BvConst(0, ACC_ROW_BITS);
                        for (size_t c = 0; c < DIM; c++) {
                            auto existingElemSp = Extract(existingRowSp, (c + 1) * INPUT_BITS - 1, c * INPUT_BITS);
                            auto existingElemAcc = Extract(existingRowAcc, (c + 1) * ACC_BITS - 1, c * ACC_BITS);
                            auto final_output2 = Ite(ReLU, Relu(c_ws), c_ws);
                            auto elemSp = (c == col) ? ResizeBv(final_output2, INPUT_BITS) : existingElemSp;
                            auto elemAcc = (c == col) ? ResizeBv(final_output2, ACC_BITS) : existingElemAcc;
                            if (c == 0) {
                                newRowSp = elemSp;
                                newRowAcc = elemAcc;
                            } else {
                                newRowSp = Concat(elemSp, newRowSp);
                                newRowAcc = Concat(elemAcc, newRowAcc);
                            }
                        }

                        scratchpad_next2 = Ite(!os_mode & (destination == BvConst(0, 1)) & col_valid,
                            scratchpad_next2.Store(writeAddrCol, newRowSp), scratchpad_next2);
                        accumulator_next2 = Ite(!os_mode & (destination == BvConst(1, 1)) & col_valid,
                            accumulator_next2.Store(writeAddrCol, newRowAcc), accumulator_next2);
                    }
                }

                // OS MODE WRITE BACK
                ExprRef scratchpad_next = scratchpad;
                ExprRef accumulator_next = accumulator;
                for (size_t row = 0; row < DIM; row++) {
                    auto writeAddr = dest_addr + row;
                    auto destRowSp = scratchpad.Load(writeAddr);
                    auto destRowAcc = accumulator.Load(writeAddr);
                    ExprRef newRowSp = BvConst(0, INPUT_ROW_BITS);
                    ExprRef newRowAcc = BvConst(0, ACC_ROW_BITS);
                    for (size_t col = 0; col < DIM; col++) {
                        auto should_transfer = Ite((BvConst(row, 16) < dest_row) & (BvConst(col, 16) < dest_col), SYMB_TRUE, SYMB_FALSE);
                        auto existingElemSp = Extract(destRowSp, (col + 1) * INPUT_BITS - 1, col * INPUT_BITS);
                        auto existingElemAcc = Extract(destRowAcc, (col + 1) * ACC_BITS - 1, col * ACC_BITS);
                        auto C_elem = sys_array[row][col]->C_reg_out;
                        auto C_elem_32 = ResizeBv(C_elem, 32);
                        auto shifted_32 = C_elem_32 >> right_shift;  
                        auto shifted_output = Extract(shifted_32, OUTPUT_BITS - 1, 0); 
                        auto final_output = Ite(ReLU, Relu(shifted_output), shifted_output);
                        if (col == 0) {
                            newRowSp = Ite(should_transfer, ResizeBv(final_output, INPUT_BITS), existingElemSp);
                            newRowAcc = Ite(should_transfer, ResizeBv(final_output, ACC_BITS), existingElemAcc);
                        } else {
                            newRowSp = Ite(should_transfer, Concat(ResizeBv(final_output, INPUT_BITS), newRowSp), Concat(existingElemSp, newRowSp));
                            newRowAcc = Ite(should_transfer, Concat(ResizeBv(final_output, ACC_BITS), newRowAcc), Concat(existingElemAcc, newRowAcc));
                        }
                    }
                    scratchpad_next = Ite(os_mode & (destination == BvConst(0, 1)) & write_cycle,
                        scratchpad_next.Store(writeAddr, newRowSp), scratchpad_next);
                    accumulator_next = Ite(os_mode & (destination == BvConst(1, 1)) & write_cycle,
                        accumulator_next.Store(writeAddr, newRowAcc), accumulator_next);
                }
                instr.SetUpdate(scratchpad, Ite(os_mode, scratchpad_next, scratchpad_next2));
                instr.SetUpdate(accumulator, Ite(os_mode, accumulator_next, accumulator_next2));

                instr.SetUpdate(cycle, cycle + BvConst(1, 32));
            }
        }

        {
            {
                // matmul.compute.accumulated
                InstrRef instr = m.NewInstr("matmul.compute.accumulated");
                auto decode = matmul_compute_accumulated;
                instr.SetDecode(funct == decode);
                auto A_scratchpad_addr = Extract(rs1, 31, 0);
                auto A_col_ = Extract(rs1, 47, 32);
                auto A_row_ = Extract(rs1, 63, 48);
                auto BD_scratchpad_addr = Extract(rs2, 31, 0);
                auto BD_col = Extract(rs2, 47, 32);
                auto BD_row = Extract(rs2, 63, 48);

                instr.SetUpdate(A_addr, A_scratchpad_addr);
                instr.SetUpdate(A_row, A_row_);
                instr.SetUpdate(A_col, A_col_);
                instr.SetUpdate(B_D_addr, BD_scratchpad_addr);
                instr.SetUpdate(B_D_row, BD_row);
                instr.SetUpdate(B_D_col, BD_col);
                instr.SetUpdate(cycle, BvConst(0, 32));
            }

            {
                // matmul.compute.accumulated step
                InstrRef instr = m.NewInstr("matmul.compute.accumulated_step");
                auto decode = matmul_compute_accumulated;
                instr.SetDecode(funct == decode & (cycle <= BvConst((2 * DIM) - 1, 32)));
                auto os_mode = (dataflow == BoolConst(false));
                auto write_cycle = os_mode & (cycle == BvConst((2 * DIM) - 1, 32));
                auto destination = Extract(dest_addr, 31, 31);

                ExprRef scratchpad_next2 = scratchpad;
                ExprRef accumulator_next2 = accumulator;

                auto A_transpose = A_T == BvConst(1, 1);
                auto B_transpose = B_T == BvConst(1, 1);
                auto ReLU = activation_func == BvConst(1, 1);

                // FIX: in_bounds must account for the active region of the *fed* operand,
                // which changes shape under transpose. A_transpose feeds A_col "rows";
                // B_transpose feeds B_D_row "columns".
                auto A_active_rows = Ite(A_transpose, ZExt(A_col, 16), A_row);
                auto B_active_cols = Ite(B_transpose, ZExt(B_D_row, 16), B_D_col);

                for (size_t row = 0; row < DIM; row++) {
                    auto is_last_row = Ite(BvConst(row + 1, 16) == dest_row, SYMB_TRUE, SYMB_FALSE);

                    for (size_t col = 0; col < DIM; col++) {
                        auto in_bounds = (BvConst(row, 16) < A_active_rows) & (BvConst(col, 16) < B_active_cols);

                        ExprRef A_in = BvConst(0, INPUT_BITS);
                        ExprRef B_D_in = BvConst(0, INPUT_BITS);

                        if (col == 0) {
                            auto k_a = cycle - BvConst(row, 32);
                            auto A_1 = Ite(cycle >= BvConst(row, 32) & k_a < ZExt(A_col, 32) & !write_cycle,
                                Extract(Lshr(scratchpad.Load(A_addr + (BvConst(row, GEMMINI_ADDR_WIDTH) * ResizeBv(A_stride, GEMMINI_ADDR_WIDTH))),
                                            ResizeBv(k_a * BvConst(INPUT_BITS, 32), INPUT_ROW_BITS)),
                                    INPUT_BITS - 1, 0),
                                BvConst(0, INPUT_BITS));
                            auto k_row = cycle - BvConst(row, 32);
                            auto A_2 = Ite(cycle >= BvConst(row, 32) & k_row < ZExt(A_row, 32) & !write_cycle,
                                Extract(scratchpad.Load(A_addr + ResizeBv(k_row, GEMMINI_ADDR_WIDTH) * ResizeBv(A_stride, GEMMINI_ADDR_WIDTH)),
                                    (row + 1) * INPUT_BITS - 1,
                                    row * INPUT_BITS),
                                BvConst(0, INPUT_BITS));

                            // FIX: "row" = output row (m) in OS, but = contraction index (k) in WS.
                            // A_1/A_2 therefore swap which one is "natural" vs "transposed" depending
                            // on mode. Keep OS's existing (already-correct) mapping; flip it for WS
                            // so A_T means the same thing (0=natural, 1=transposed) in both modes.
                            auto A_in_os = Ite(A_transpose, A_2, A_1);
                            auto A_in_ws = Ite(A_transpose, A_1, A_2);
                            A_in = Ite(os_mode, A_in_os, A_in_ws);
                        } else {
                            A_in = sys_array[row][col - 1]->A_reg;
                        }

                        if (row == 0) {
                            auto k_b = cycle - BvConst(col, 32);
                            auto B_1 = Ite(cycle >= BvConst(col, 32) & k_b < ZExt(B_D_row, 32) & !write_cycle,
                                Extract(scratchpad.Load(B_D_addr + k_b),
                                    (col + 1) * INPUT_BITS - 1,
                                    col * INPUT_BITS),
                                BvConst(0, INPUT_BITS));
                            // FIX: keep col-based skew and col-pitched address (mirror of A_2's fix);
                            // was incorrectly keyed off `row` (which is 0 in this scope).
                            auto k_col = cycle - BvConst(col, 32);
                            auto B_2 = Ite(cycle >= BvConst(col, 32) & k_col < ZExt(B_D_col, 32) & !write_cycle,
                                Extract(Lshr(scratchpad.Load(B_D_addr + BvConst(col, GEMMINI_ADDR_WIDTH)),
                                            ResizeBv(k_col * BvConst(INPUT_BITS, 32), INPUT_ROW_BITS)),
                                    INPUT_BITS - 1, 0),
                                BvConst(0, INPUT_BITS));
                            B_D_in = Ite(B_transpose, B_2, B_1);
                        } else {
                            B_D_in = sys_array[row - 1][col]->B_D_reg;
                        }

                        instr.SetUpdate(sys_array[row][col]->A_reg, Ite(in_bounds & !write_cycle, A_in, sys_array[row][col]->A_reg));
                        instr.SetUpdate(sys_array[row][col]->B_D_reg, Ite(in_bounds & !write_cycle, B_D_in, sys_array[row][col]->B_D_reg));

                        auto weight = Ite(os_mode, ZExt(B_D_in, ACC_BITS), sys_array[row][col]->stationary_reg);
                        auto product = ZExt(A_in, ACC_BITS) * weight;

                        auto stat_updated = sys_array[row][col]->stationary_reg + product;
                        instr.SetUpdate(sys_array[row][col]->stationary_reg,
                            Ite(os_mode & in_bounds & !write_cycle, stat_updated, sys_array[row][col]->stationary_reg));
                        auto c_os = Extract(stat_updated, OUTPUT_BITS - 1, 0);

                        // D (psum) path: intentionally independent of A_transpose/B_transpose.
                        // config_ex has no D-transpose option, so this must never route
                        // through the A_2/B_2 transpose muxes above.
                        ExprRef psum_in = BvConst(0, ACC_BITS);
                        if (row == 0) {
                            auto k_p = cycle - BvConst(col, 32);
                            psum_in = Ite(cycle >= BvConst(col, 32) & k_p < ZExt(B_D_row, 32),
                                ZExt(Extract(scratchpad.Load(B_D_addr + k_p),
                                         (col + 1) * INPUT_BITS - 1, col * INPUT_BITS),
                                    ACC_BITS),
                                BvConst(0, ACC_BITS));
                        } else {
                            psum_in = ZExt(sys_array[row - 1][col]->C_reg_out, ACC_BITS);
                        }
                        auto c_ws = Extract(product + psum_in, OUTPUT_BITS - 1, 0);

                        instr.SetUpdate(sys_array[row][col]->C_reg_out,
                            Ite(in_bounds & !write_cycle, Ite(os_mode, c_os, c_ws), sys_array[row][col]->C_reg_out));

                        auto row_off = BvConst(row, 32);
                        auto col_off = BvConst(col, 32);
                        auto instance = cycle - row_off - col_off;
                        auto col_valid = is_last_row
                            & (cycle >= row_off + col_off)
                            & (instance < ZExt(dest_row, 32)) // Could underflow?
                            & Ite((BvConst(col, 16) < dest_col), SYMB_TRUE, SYMB_FALSE);
                        auto writeAddrCol = dest_addr + instance;

                        auto existingRowSp = scratchpad_next2.Load(writeAddrCol);
                        auto existingRowAcc = accumulator_next2.Load(writeAddrCol);
                        ExprRef newRowSp = BvConst(0, INPUT_ROW_BITS);
                        ExprRef newRowAcc = BvConst(0, ACC_ROW_BITS);
                        for (size_t c = 0; c < DIM; c++) {
                            auto existingElemSp = Extract(existingRowSp, (c + 1) * INPUT_BITS - 1, c * INPUT_BITS);
                            auto existingElemAcc = Extract(existingRowAcc, (c + 1) * ACC_BITS - 1, c * ACC_BITS);
                            auto final_output2 = Ite(ReLU, Relu(c_ws), c_ws);
                            auto elemSp = (c == col) ? ResizeBv(final_output2, INPUT_BITS) : existingElemSp;
                            auto elemAcc = (c == col) ? ResizeBv(final_output2, ACC_BITS) : existingElemAcc;
                            if (c == 0) {
                                newRowSp = elemSp;
                                newRowAcc = elemAcc;
                            } else {
                                newRowSp = Concat(elemSp, newRowSp);
                                newRowAcc = Concat(elemAcc, newRowAcc);
                            }
                        }

                        scratchpad_next2 = Ite(!os_mode & (destination == BvConst(0, 1)) & col_valid,
                            scratchpad_next2.Store(writeAddrCol, newRowSp), scratchpad_next2);
                        accumulator_next2 = Ite(!os_mode & (destination == BvConst(1, 1)) & col_valid,
                            accumulator_next2.Store(writeAddrCol, newRowAcc), accumulator_next2);
                    }
                }

                // OS MODE WRITE BACK
                ExprRef scratchpad_next = scratchpad;
                ExprRef accumulator_next = accumulator;
                for (size_t row = 0; row < DIM; row++) {
                    auto writeAddr = dest_addr + row;
                    auto destRowSp = scratchpad.Load(writeAddr);
                    auto destRowAcc = accumulator.Load(writeAddr);
                    ExprRef newRowSp = BvConst(0, INPUT_ROW_BITS);
                    ExprRef newRowAcc = BvConst(0, ACC_ROW_BITS);
                    for (size_t col = 0; col < DIM; col++) {
                        auto should_transfer = Ite((BvConst(row, 16) < dest_row) & (BvConst(col, 16) < dest_col), SYMB_TRUE, SYMB_FALSE);
                        auto existingElemSp = Extract(destRowSp, (col + 1) * INPUT_BITS - 1, col * INPUT_BITS);
                        auto existingElemAcc = Extract(destRowAcc, (col + 1) * ACC_BITS - 1, col * ACC_BITS);
                        auto C_elem = sys_array[row][col]->C_reg_out;
                        auto C_elem_32 = ResizeBv(C_elem, 32);
                        auto shifted_32 = C_elem_32 >> right_shift;  
                        auto shifted_output = Extract(shifted_32, OUTPUT_BITS - 1, 0); 
                        auto final_output = Ite(ReLU, Relu(shifted_output), shifted_output);
                        if (col == 0) {
                            newRowSp = Ite(should_transfer, ResizeBv(final_output, INPUT_BITS), existingElemSp);
                            newRowAcc = Ite(should_transfer, ResizeBv(final_output, ACC_BITS), existingElemAcc);
                        } else {
                            newRowSp = Ite(should_transfer, Concat(ResizeBv(final_output, INPUT_BITS), newRowSp), Concat(existingElemSp, newRowSp));
                            newRowAcc = Ite(should_transfer, Concat(ResizeBv(final_output, ACC_BITS), newRowAcc), Concat(existingElemAcc, newRowAcc));
                        }
                    }
                    scratchpad_next = Ite(os_mode & (destination == BvConst(0, 1)) & write_cycle,
                        scratchpad_next.Store(writeAddr, newRowSp), scratchpad_next);
                    accumulator_next = Ite(os_mode & (destination == BvConst(1, 1)) & write_cycle,
                        accumulator_next.Store(writeAddr, newRowAcc), accumulator_next);
                }
                instr.SetUpdate(scratchpad, Ite(os_mode, scratchpad_next, scratchpad_next2));
                instr.SetUpdate(accumulator, Ite(os_mode, accumulator_next, accumulator_next2));

                instr.SetUpdate(cycle, cycle + BvConst(1, 32));
            }
        }
    }
}

}
