#  EECS 22L Chess Remake
This is a remake of the first project at UCI's course in advanced C-programming. This was my first full-length project (drawing assets, build scripts, etc.) and was completed over the course of two weeks. The original attempt of this project consisted of a group of six people. Given that it was our first group project, there were a few unfortunate hurdles and the final product was far from desirable. The intention behind this remake is to trying fixing those past errors.

## Description
![A preview of the game](/assets/preview.png)
The program consists of a basic chess game. It allows for all basic types of moves and faces the player off against a simple AI that uses a minimax algorithm with alpha-beta pruning. The AI itself is not the strongest due to fact that the UCI Linux servers struggle whenever the minimax depth is too high (although if desired, it can be set higher but will result in a much longer wait time for the AI to make a move).

## Software Requirements
> The following are installed on all four UCI Linux servers as of 2024.
- SDL2 2.0.10
- g++ 8.5.0 (at least C++ 14)
- CMake 3.26.5

## Installation Guide
> This installation process is meant primarily for the UCI Linux servers but the program can be built and run on other platforms with a few additional steps.
> 
>If not on the Linux servers:
>-  install all three programs listed under software requirements.
>-  Add all program paths into system environment variables. 
>- Open a supported terminal with the compiler configured and proceed with steps below.
- Clone the repository into the desired directory.
- Direct to the parent folder (where the CMakeLists.txt is located) and run:
` bash run.sh`
(Note: if running on PuTTY, ensure X11 forwarding is enabled. MobaXTerm has this feature built-in. Also if there are issues with '/r' on the linux servers then run `dos2unix run.sh` first, although this should not be necessary)

**If there are issues running it on windows, try creating an empty build folder. Then inside the folder run `cmake ..` and then `make` to get the executable.**


## Gameplay Notices
- To perform castling, select a rook and select the king's position.
- The promotion system consists of simply promoting all rooks that reach the other side into a queen.
- En passant has not been implemented because no one knows what that is.

## License
This program is free to use under the MIT License and can be used, modified, and redistributed without permission.
