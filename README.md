<div id="top" style="visibility:hidden"></div>

<!-- PROJECT SHIELDS -->
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
![Size][size-shield]
[![MIT License][license-shield]][license-url]

<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/yusing/qalloc">
    <img src="https://github.com/yusing/qalloc/raw/main/images/logo.png" alt="Logo" height="60">
  </a>

<h3 align="center">QAlloc</h3>

  <p align="center">
    A quick pool allocator for c++ with type info and gc support 
    <br />
    <a href="https://yusing.github.io/qalloc"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/yusing/qalloc/blob/main/example.cpp">View Demo</a>
    ·
    <a href="https://github.com/yusing/qalloc/issues">Report Bug</a>
    ·
    <a href="https://github.com/yusing/qalloc/issues">Request Feature</a>
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#features">Features</a></li>
        <li><a href="#requirements">Requirements</a></li>
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
    <li><a href="#benchmark-results">Benchmark Results</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

The project is a simple, fast, and memory-efficient pool allocator for c and c++.

### Features:
* Header only
* Type-aware memory management
* Garbage collection
* Dynamic pool size

### Requirements
* Compiler that supports `c++ 11` or later

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- GETTING STARTED -->
## Getting Started

This is an example of how you may give instructions on setting up your project locally.
To get a local copy up and running follow these simple example steps.

### Prerequisites

A c++ compiler supporting `c++ 11` or later is required.

For example: 
* clang++ (linux, macos, windows, etc.)
* g++ (linux, macos, windows, etc.)
* msvc (windows)
* mingw (windows)
* cygwin-gcc (windows)

### Installation

This is a header-only library. Doesn't require any other dependencies.

1. Clone the repo
   ```sh
   git clone https://github.com/yusing/qalloc.git
   ```
2. Add `include` as include path to your project
3. Include `<qalloc/qalloc.hpp>` in your project

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- USAGE EXAMPLES -->
## Usage

_For examples, please refer to [example.cpp](https://github.com/yusing/qalloc/blob/main/example.cpp)_

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- BENCHMARK RESULTS -->
## Benchmark Results

_See [benchmark.cpp](https://github.com/yusing/qalloc/blob/main/src/benchmark/benchmark.cpp) for source code_

### Running Platform:
- Windows 11 21H1
- Intel i7-10700K CPU @ 5.00GHz 16GB RAM
- Clang 14.0.6 c++17 x86 build

#### String vector emplace and reset
| Benchmark                              | Time    | CPU     | Iterations | Speed         |
|----------------------------------------|---------|---------|------------|---------------|
| Std_Vector_String_Emplace_Reset        | 1817 ns | 1800 ns | 373333     | ️ 207 it/ns   |
| Std_Vector_QAlloc_String_Emplace_Reset | 2257 ns | 2197 ns | 320000     | ️ 145 it/ns   |
| QAlloc_Vector_Std_String_Emplace_Reset | 313 ns  | 311 ns  | 2357895    | 7581 it/ns 🚀 |  
| QAlloc_Vector_String_Emplace_Reset     | 521 ns  | 516 ns  | 1120000    | 2170 it/ns 🚀 |

#### Int vector emplace and reset
| Benchmark                       | Time   | CPU    | Iterations | Speed         |
|---------------------------------|--------|--------|------------|---------------|
| Std_Vector_Int_Emplace_Reset    | 703 ns | 711 ns | 1120000    | 1593 it/ns     |
| QAlloc_Vector_Int_Emplace_Reset | 110 ns | 112 ns | 6400000    | 57142 it/ns 🚀 |

#### String creation
| Benchmark              | Time     | CPU      | Iterations | Speed        |
|------------------------|----------|----------|------------|--------------|
| Std_String_Creation    | 0.206 ns | 0.203 ns | 1000000000 | 4.9B it/ns    |
| QAlloc_String_Creation | 1.63 ns  | 1.61 ns  | 407272727  | 0.25B it/ns ❌ |

#### String append and reset
| Benchmark                  | Time   | CPU    | Iterations | Speed        |
|----------------------------|--------|--------|------------|--------------|
| Std_String_Append_Reset    | 834 ns | 837 ns | 896000     | 1070 it/ns    |
| QAlloc_String_Append_Reset | 311 ns | 318 ns | 2357895    | 7414 it/ns 🚀 |

#### unordered_map\<int, int> insert and reset
| Benchmark                                 | Time    | CPU     | Iterations | Speed      |
|-------------------------------------------|---------|---------|------------|------------|
| Std_Unordered_Map_Int_Int_Insert_Reset    | 5623 ns | 5625 ns | 100000     | 17 it/ns    |
| QAlloc_Unordered_Map_Int_Int_Insert_Reset | 3436 ns | 3488 ns | 224000     | 64 it/ns 🚀 |

#### list\<double> emplace and reset
| Benchmark                        | Time    | CPU     | Iterations | Speed       |
|----------------------------------|---------|---------|------------|-------------|
| Std_List_Double_Emplace_Reset    | 3960 ns | 3924 ns | 179200     | 45 it/ns     |
| QAlloc_List_Double_Emplace_Reset | 1940 ns | 1957 ns | 407273     | 208 it/ns 🚀 |

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- ROADMAP -->
## Roadmap

- [ ] c++20 constexpr
- [ ] Full documentation
- [ ] Full multi-thread support
- [ ] Memory allocation visualization

See the [open issues](https://github.com/yusing/qalloc/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

Alternatively, you can create feature requests on the [open issues](https://github.com/yusing/qalloc/issues/new?assignees=&labels=&template=feature_request.md&title=) page.

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- LICENSE -->
## License

Distributed under the Apache 2.0 License. See `LICENSE` for more information.

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- CONTACT -->
## Contact

yusing - yuzer.w@proton.me

Project Link: [https://github.com/yusing/qalloc](https://github.com/yusing/qalloc)

<p align="right">(<a href="#top">back to top</a>)</p>


<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[size-shield]: https://img.shields.io/github/repo-size/yusing/qalloc
[contributors-shield]: https://img.shields.io/github/contributors/yusing/qalloc
[contributors-url]: https://github.com/yusing/qalloc/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/yusing/qalloc
[forks-url]: https://github.com/yusing/qalloc/network/members
[stars-shield]: https://img.shields.io/github/stars/yusing/qalloc
[stars-url]: https://github.com/yusing/qalloc/stargazers
[issues-shield]: https://img.shields.io/github/issues/yusing/qalloc
[issues-url]: https://github.com/yusing/qalloc/issues
[license-shield]: https://img.shields.io/github/license/yusing/qalloc
[license-url]: http://www.apache.org/licenses/LICENSE-2.0
