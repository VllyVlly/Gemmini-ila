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


<!-- ROADMAP -->
## Roadmap

- [x] `matmul.preload` — WS-mode transpose handling for stationary weights
- [x] `matmul.compute.preloaded_step` — OS/WS cycle-by-cycle stepping with A/B transpose
- [x] Test suite: OS/WS × A-only/B-only/AB-transpose at DIM=2
- [ ] Extend test coverage to larger array dimensions
- [ ] Model additional Gemmini instructions (e.g. `mvin`, `mvout`, `config_st`)
- [ ] End-to-end equivalence checking against RTL



<!-- CONTRIBUTING -->
## Contributing

Contributions are welcome. If you have a suggestion, please fork the repo and open a pull request, or open an issue with the "enhancement" tag.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request


<!-- LICENSE -->
## License

Distributed under the project_license. See `LICENSE.txt` for more information.


<!-- CONTACT -->
## Contact

Your Name - [@twitter_handle](https://twitter.com/twitter_handle) - email@email_client.com

Project Link: [https://github.com/github_username/gemmini-ila](https://github.com/github_username/gemmini-ila)


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