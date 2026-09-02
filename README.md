<a id="readme-top"></a>

<!-- PROJECT SHIELDS -->


<!-- PROJECT LOGO -->
<br />
<div align="center">
  <h3 align="center">Gemmini ILA</h3>

  <p align="center">
    A formal Instruction-Level Abstraction (ILA) model of the Gemmini systolic array accelerator, built with ILAng.
  </p>
</div>


<!-- ABOUT THE PROJECT -->
## About

This repository contains a formal ILA (Instruction-Level Abstraction) model of the [Gemmini](https://github.com/ucb-bar/gemmini) systolic array accelerator, built using [ILAng](https://github.com/Bo-Yuan-Huang/ILAng).

The model specifies Gemmini's instruction semantics at the abstraction level needed for formal verification, precise enough to capture cycle-level systolic array behavior, but abstracted away from RTL implementation detail. 


### Built With

* [ILAng](https://github.com/Bo-Yuan-Huang/ILAng) — ILA modeling and verification framework
* C++17
* CMake
* [Z3](https://github.com/Z3Prover/z3) — SMT solver backend


<!-- GETTING STARTED -->
## Getting Started

### Prerequisites

ILAng requires CMake (3.9.6 or above) and a compiler with C++17 support. To install dependencies on Debian-based Linux:

  ```sh
  apt-get install bison flex z3 libz3-dev
  ```

### Installation 

This repo does not vendor or track ILAng directly — you'll need to clone it separately alongside this repo before building.

1. Clone this repo
   ```sh
   git clone https://github.com/VllyVlly/Gemmini-ila
   cd gemmini-ila
   ```
2. Clone ILAng at the root of this repo
   ```sh
   git clone https://github.com/Bo-Yuan-Huang/ILAng
   ```
   Your directory structure should now look like:
   ```
   gemmini-ila/
   ├── ILAng/
   ├── src/
   ├── CMakeLists.txt
   └── ...
   ```
3. Build ILAng
   ```sh
   cd ILAng
   mkdir build && cd build
   cmake ..
   make -j$(nproc)
   sudo make install
   cd ../..
   ```
4. Build the Gemmini ILA model
   ```sh
   mkdir build && cd build
   cmake ..
   make -j$(nproc)
   ```
5. Run the test suite
   ```sh
   ./test_gemmini_ila
   ```


<!-- MODELLING NOTES -->
## Modelling Notes

In this model, DRAM elements are stored in sequential memory addresses separated by a single-element gap (stride = 1) along each row. 

However, instruction configurations still utilize `memory_stride` to define the address offset between consecutive **rows** of the matrix:

* **Element-to-Element (Row-wise):** Contiguous elements within the same row occupy adjacent target addresses, spaced by a default step of 1.
* **Row-to-Row:** The starting address of row $k+1$ is determined relative to row $k$ using `memory_stride`:
$$\text{Addr}(\text{row}_{k+1}) = \text{Addr}(\text{row}_k) + \text{memory\_stride}$$

<br>
</br>

The `Gemmini` class requires a `cfg` configuration struct during initialization to parameterize the hardware layout, memory capacities, and data types of the model:

```cpp
// Instantiate with custom configurations
gemmini::cfg my_config(
    16, 16, 1, 1,            // tileRows, tileColumns, meshRows, meshColumns
    4, 64 * 1024, 128 * 1024, // sp_banks, sp_capacity, acc_capacity
    gemmini::DataType::INT8,  // inputType
    gemmini::DataType::INT16, // outputType
    gemmini::DataType::INT32  // accType
);


// Instantiate the Gemmini ILA model
gemmini::Gemmini model(my_config, "Gemmini_Instance");
```

<br>
</br>

The model provides two distinct implementations for matrix multiplication compute instructions (`matmul_compute_preloaded`, `matmul_compute_accumulated`, and `matmul_compute_atomic`) to support both cycle-accurate analysis and high-level behavioral verification.

| Feature | Stepped Version (`stepped`) | Atomic Version (`atomic`) |
| :--- | :--- | :--- |
| **Execution Style** | Cycle-by-cycle evaluation | Single-step execution |
| **State Progression** | Updates processing element (PE) states, and `cycle` counters step-by-step | Computes the full matrix multiplication result instantaneously in one logical state transition |


Both versions share the exact same instruction decoding logic, configuration register states, and underlying systolic array arithmetic definitions, allowing direct equivalence checking between the two execution models.

<!-- ROADMAP -->
## Roadmap

- [x] Data movement instructions — `mvin`, `mvout`
- [x] Config instructions — `config_ex`, `config_mvin`, `config_mvout`
- [x] Core matmul sequence instructions — `matmul.preload`, `matmul.compute.preloaded`, `matmul.compute.accumulated`
- [ ] Add support for different data types other than unsigned integer of different bit-widths
- [ ] Model additional Gemmini instructions (e.g. loop instructions, flush)
- [ ] Fix equivalence check of atomic and stepped version of compute instructions


<!-- CONTRIBUTING -->
## Contributing

Contributions are welcome. If you have a suggestion, please fork the repo and open a pull request, or open an issue with the "enhancement" tag.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request


<!-- CONTACT -->
## Contact

VllyVlly - virellelly578@gmail.com

Project Link: https://github.com/VllyVlly/Gemmini-ila


<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

* [Gemmini](https://github.com/ucb-bar/gemmini)
* [ILAng](https://github.com/Bo-Yuan-Huang/ILAng)



<!-- MARKDOWN LINKS & IMAGES -->
[contributors-shield]: https://img.shields.io/github/contributors/github_username/gemmini-ila.svg?style=for-the-badge
[contributors-url]: https://github.com/github_username/gemmini-ila/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/github_username/gemmini-ila.svg?style=for-the-badge
[forks-url]: https://github.com/github_username/gemmini-ila/network/members
[stars-shield]: https://img.shields.io/github/stars/github_username/gemmini-ila.svg?style=for-the-badge
[stars-url]: https://github.com/github_username/gemmini-ila/stargazers
[issues-shield]: https://img.shields.io/github/issues/github_username/gemmini-ila.svg?style=for-the-badge
[issues-url]: https://github.com/github_username/gemmini-ila/issues
[license-shield]: https://img.shields.io/github/license/github_username/gemmini-ila.svg?style=for-the-badge
[license-url]: https://github.com/github_username/gemmini-ila/blob/master/LICENSE.txt