<div id="top"></div>



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
    <img src="images/logo.png" alt="Logo" height="60">
  </a>

<h3 align="center">QAlloc</h3>

  <p align="center">
    A quick pool allocator for c++ with type info and gc support 
    <br />
    <a href="https://github.com/yusing/qalloc/tree/main/doc"><strong>Explore the docs »</strong></a>
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

_For examples, please refer to [example.cpp](example.cpp)_

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- ROADMAP -->
## Roadmap

- [ ] Full documentation
- [ ] Full multi-thread support

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