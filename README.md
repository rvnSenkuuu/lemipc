# Lemipc

![image](./Screenshot.png)

A multi-process game in C where independent player processes compete on a shared map, synchronized through System V IPC (shared memory and semaphores).

The game can be visualized directly in the terminal or through a graphical interface built with raylib.

## Game Rules

Teams of independent processes fight on a shared 2D board. Each player moves one cell per turn toward the nearest enemy, and is eliminated when surrounded by two or more enemies of the same team. The last team standing wins.

## Usage

```bash
$> make
$> ./launch.sh
```

## Configuration

The size of the 2D board can be adjusted through the `BOARD_WIDTH` and `BOARD_HEIGHT` defines in [incs/lemipc.h](incs/lemipc.h). Simply update these values and recompile.