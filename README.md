# &#8192; <div align="center"> <img src="https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white" width="80" height="28"> <img src="https://img.shields.io/badge/CMake-%23B44666.svg?style=for-the-badge&logo=cmake&logoColor=white" width="80" height="28">  <img src="https://img.shields.io/badge/v0.1b-%23F1502F.svg?style=for-the-badge&logo=git&logoColor=white" width="80" height="28">  <img src="https://img.shields.io/badge/GPL3.0-%23555555.svg?style=for-the-badge&logo=gnu&logoColor=white" width="80" height="28"></div>


<h align="center">

        ██████╗   ██╗  ██████╗   ███████╗   ██████╗         ████████╗  ██╗   ██╗  ██╗  ████████╗
        ██╔══██╗  ██║  ██╔══██╗  ██╔════╝  ██╔════╝         ╚══██╔══╝  ██║   ██║  ██║  ╚══██╔══╝
        ██║  ██║  ██║  ██████╔╝  █████╗    ██║      █████╗     ██║     ██║   ██║  ██║     ██║
        ██║  ██║  ██║  ██╔══██╗  ██╔══╝    ██║      ╚════╝     ██║     ██║   ██║  ██║     ██║
        ██████╔╝  ██║  ██║  ██║  ███████╗  ╚██████╗            ██║     ╚██████╔╝  ██║     ██║
        ╚═════╝   ╚═╝  ╚═╝  ╚═╝  ╚══════╝   ╚═════╝            ╚═╝      ╚═════╝   ╚═╝     ╚═╝
</h>
<div align="center"><i>a simple way to visualize files in your terminal.</i></div>
<br>



<div align = "center" > 
    <table >
      <tr><td><a href="#overview">Overview</a></td></tr>
      <tr><td><a href="#demo">Demo</a></td></tr>
      <tr><td><a href="#installation">Installation</a></td></tr>
      <tr><td><a href="#dependencies">Dependencies</a></td></tr>
      <tr><td><a href="#contributing">Contributing</a></td></tr>
      <tr><td><a href="#acknowledgements">Acknowledgements</a></td></tr>
      <tr><td><a href="#license">License</a></td></tr>
    </table>
</div>


## Overview

`direc-tuit` is a modern, terminal-based file explorer, with lightweight navigation and vim-like keybinds. designed for speed, clarity and compatibility across platforms.

## Demo 
![Demo](https://dt-site-nine.vercel.app/assets/demo1-CSPNytxM.gif)



## Installation 
Download, unpack and run the [respective binary]("https://github.com/beaualbritton/direc-tuit/releases") </a>for your platform. 
### Mac:
  ```bash
  tar -xzvf dt-v-0.1b-macos-universal.tar.gz
  ./dir-tuit
  ```

### Linux-x86:
  ```bash
  tar -xzvf dt-v-0.1b-linux-x86.tar.gz
  ./dir-tuit
  ```
### Linux-arm64:
  ```bash
  tar -xzvf dt-v-0.1b-linux-arm64.tar.gz
  ./dir-tuit
  ```
### Windows:
  ```powershell
  #powershell commands are weird
  Expand-Archive dt-v-0.1b-win-x86.zip
  .\dir-tuit.exe
  ```

### Building from Source
If you're not familiar with CMake, the process is fairly intuitive.
```bash
#clone/fork the repo, then build with CMake. that's it :^)
git clone https://github.com/beaualbritton/direc-tuit
cmake -S . -B build -G Ninja
cmake --build build
#direc-tuit can be found in ./build/dir-tuit
```

## Dependencies
A [NerdFont](https://www.nerdfonts.com/) installed and configured for your [terminal environment](https://github.com/ryanoasis/nerd-fonts?tab=readme-ov-file#font-installation).

If you're building from source
* A C++23 compatible compiler
* CMake 3.14

## Contributing
Contributions are more than welcome. Have an idea for a feature, or know how to fix a bug? Fork the repo and open a Pull Request.

## Acknowledgements
Special thanks to the maintainers of [FTXUI](https://github.com/ArthurSonzogni/FTXUI/) & [tomlplusplus](https://github.com/marzer/tomlplusplus) - two excellent libraries that made this project possible.

## License
**Released under GNU GPL-3.**

*See [LICENSE](LICENSE) for details.*


