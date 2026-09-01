<a id="readme-top"></a>

<!-- PROJECT SHIELDS -->
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![project_license][license-shield]][license-url]



<!-- PROJECT LOGO -->
<br />
<div align="center">
  <h3 align="center">Gemmini ILA</h3>

  <p align="center">
    A formal Instruction-Level Abstraction (ILA) model of the Gemmini systolic array accelerator, built with ILAng.
    <br />
    <a href="https://github.com/github_username/gemmini-ila"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/github_username/gemmini-ila/issues/new?labels=bug&template=bug-report---.md">Report Bug</a>
    &middot;
    <a href="https://github.com/github_username/gemmini-ila/issues/new?labels=enhancement&template=feature-request---.md">Request Feature</a>
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#modeling-notes">Modeling Notes</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

This repository contains a formal ILA (Instruction-Level Abstraction) model of the [Gemmini](https://github.com/ucb-bar/gemmini) systolic array accelerator, built using [ILAng](https://github.com/Bo-Yuan-Huang/ILAng).

The model specifies Gemmini's instruction semantics at the abstraction level needed for formal verification — precise enough to capture cycle-level systolic array behavior, but abstracted away from RTL implementation detail. The current focus is on **matrix multiplication instructions with transpose support**, specifically how `config_ex` parameters (`A_transpose`, `B_transpose`) propagate into compute behavior across Gemmini's two dataflow modes:

- **Output-Stationary (OS) mode** — B is streamed in via `B_D_in`, so `B_T` affects the streamed input directly.
- **Weight-Stationary (WS) mode** — B comes from `stationary_reg`, loaded by `matmul.preload`, so `B_T` must be applied at load time.

D (the accumulator seed) is never transposed — `config_ex` has no transpose option for it, and its path through `psum_in` is architecturally decoupled from the transpose muxes.

Instructions currently modeled and under active verification:
- `matmul.compute.preloaded_step` — cycle-by-cycle systolic array stepping
- `matmul.preload` — loads stationary weights or accumulator seeds into PE registers

<p align="right">(<a href="#readme-top">back to top</a>)</p>



### Built With

* [ILAng](https://github.com/Bo-Yuan-Huang/ILAng) — ILA modeling and verification framework
* C++17
* CMake
* [Z3](https://github.com/Z3Prover/z3) — SMT solver backend

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- GETTING STARTED -->
## Getting Started

### Prerequisites

ILAng requires CMake (3.9.6 or above) and a compiler with C++17 support. To install dependencies on Debian-based Linux:

  ```sh
  apt-get install bison flex z3 libz3-dev
  ```

### Installation

1. Clone the repo
   ```sh
   git clone https://github.com/github_username/gemmini-ila.git
   cd gemmini-ila
   ```
2. Build with CMake
   ```sh
   mkdir build && cd build
   cmake ..
   make -j$(nproc)
   ```
3. Run the test suite
   ```sh
   ./test_gemmini_ila
   ```

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- USAGE EXAMPLES -->
## Usage

The model can be exercised through its test suite, which covers OS and WS modes with A-only, B-only, and AB-transpose cases at small array dimensions (e.g. DIM=2). Each test constructs an instruction sequence, steps the ILA, and checks final scratchpad/accumulator state against expected values.

```sh
./build/test_gemmini_ila --gtest_filter=*Transpose*
```

_For details on the ILA specification format, see the [ILAng documentation](https://github.com/Bo-Yuan-Huang/ILAng)._

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- MODELING NOTES -->
## Modeling Notes

A few non-obvious conventions worth documenting for contributors:

- **A/B transpose asymmetry**: A's default scratchpad read is already logically transposed (physical row indexes the contraction dimension `k`), so `A_T=1` actually means "read as natural row-major" — the opposite of the naive expectation. B has no such pre-transposition, so `B_T=1` applies a genuine transpose.
- **`B_T` is gated on WS mode** (`!os_mode`) in `matmul.preload`, since B follows a different path in OS mode.
- **Index variable scope**: `row` and `col` are not both nonzero in the same instruction phase — always check which is zero in a given scope before using it as an index.
- Transpose selection is implemented with `Ite` expressions to keep spec edits non-destructive and auditable.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ROADMAP -->
## Roadmap

- [x] `matmul.preload` — WS-mode transpose handling for stationary weights
- [x] `matmul.compute.preloaded_step` — OS/WS cycle-by-cycle stepping with A/B transpose
- [x] Test suite: OS/WS × A-only/B-only/AB-transpose at DIM=2
- [ ] Extend test coverage to larger array dimensions
- [ ] Model additional Gemmini instructions (e.g. `mvin`, `mvout`, `config_st`)
- [ ] End-to-end equivalence checking against RTL

See the [open issues](https://github.com/github_username/gemmini-ila/issues) for a full list of proposed features and known issues.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTRIBUTING -->
## Contributing

Contributions are welcome. If you have a suggestion, please fork the repo and open a pull request, or open an issue with the "enhancement" tag.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<p align="right">(<a href="#readme-top">back to top</a>)</p>

### Top contributors:

<a href="https://github.com/github_username/gemmini-ila/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=github_username/gemmini-ila" alt="contrib.rocks image" />
</a>



<!-- LICENSE -->
## License

Distributed under the project_license. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTACT -->
## Contact

Your Name - [@twitter_handle](https://twitter.com/twitter_handle) - email@email_client.com

Project Link: [https://github.com/github_username/gemmini-ila](https://github.com/github_username/gemmini-ila)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

* [Gemmini](https://github.com/ucb-bar/gemmini)
* [ILAng](https://github.com/Bo-Yuan-Huang/ILAng)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



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