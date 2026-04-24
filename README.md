# ChemReact3D — 3D Chemical Reaction Exposition using OpenGL

An interactive 3D visualiser for organic chemistry reactions built with OpenGL and freeglut.
Displays reactants and products using CPK-coloured atom spheres, bond cylinders, electron-orbit animations, and real-time lighting with floor shadows.

---

## Course

**CS-302N: Computer Graphics and Visualization**

---

## Team Members

| # | Name | Roll Number |
|---|---|---|
| 1 | Nandini Kumari | 230001056 |
| 2 | Shaik Sultana | 230001071 |
| 3 | Tripti Anand | 230001078 |

---

## Acknowledgement

This project builds upon the base code from the repository **Opengl-3D-Chemical-Reaction-View** by **im-Amitto**.
Original source: https://github.com/im-Amitto/Opengl-3D-Chemical-Reaction-View

We extended and modified the original implementation by adding new visual features, improving rendering quality, enhancing interactivity, and refining the overall user experience.

---

## Project Structure

```text
Group_ID_ChemReact3D/
├── ChemReact3D.cpp          # Main source file
├── lib/
│   ├── RgbImage.h           # BMP texture loader header
│   └── RgbImage.cpp         # BMP texture loader implementation
├── XYZ/
│   ├── benzene.xyz
│   ├── h2so4.xyz
│   ├── benzenesulfonic_acid.xyz
│   ├── hydrogen.xyz
│   ├── cyclohexane.xyz
│   ├── bromine.xyz
│   ├── bromobenzene.xyz
│   ├── c4h8.xyz
│   ├── chcl3.xyz
│   ├── c4h6.xyz
│   ├── ch3cl.xyz
│   └── toulene.xyz
├── bmp/
│   ├── hydrogen.bmp
│   ├── carbon.bmp
│   ├── nitrogen.bmp
│   ├── oxygen.bmp
│   ├── sulfur.bmp
│   ├── chlorine.bmp
│   └── bromine.bmp
├── screenshots/
├── README.md
└── run.sh
```

---

## Dependencies

| Library | Purpose | Install Command (Ubuntu/Debian) |
|---|---|---|
| `g++` | C++ compiler | `sudo apt install build-essential` |
| `freeglut3` | OpenGL windowing & input | `sudo apt install freeglut3-dev` |
| `libGL` / `libGLU` | OpenGL / GLU rendering | included with `freeglut3-dev` |

---

## Installation (Linux)

```bash
sudo apt update
sudo apt install -y build-essential freeglut3-dev libgl1-mesa-dev libglu1-mesa-dev
```

```bash
unzip Group_ID_ChemReact3D.zip
cd Group_ID_ChemReact3D
```

```bash
g++ -w main.cpp  -lglut -lGL -lGLU -o ChemReact3D
```

```bash
./ChemReact3D
```

---

## Controls

### Mouse

| Action | Result |
|---|---|
| Left-drag | Rotate scene |
| Middle-drag | Pan scene |
| Scroll wheel | Zoom in / out |
| Left-click | Select atom / exit title screen |
| Right-click | Open context menu |

### Keyboard

| Key | Action |
|---|---|
| `W A S D` | Pan up / left / down / right |
| `Q` / `T` | Zoom in / out |
| `N` / `P` | Next / Previous reaction |
| `SPACE` | Toggle scene auto-spin |
| `O` | Toggle per-atom self-spin |
| `B` | Toggle Bohr electron orbits |
| `F` | Toggle fog |
| `E` | Toggle electron shells |
| `V` | Toggle environment map |
| `L` | Toggle atom labels |
| `H` | Toggle floor shadows |
| `G` | Toggle Perspective / Parallel projection |
| `[` / `]` | Widen / narrow orthographic view |
| `+` / `-` | Increase / decrease spin speed |
| `ESC` | Quit |

---

## Reactions Included

| # | Reaction | Reactant A | Reactant B | Product |
|---|---|---|---|---|
| 1 | Sulphonation | Benzene | H₂SO₄ | Benzenesulfonic Acid |
| 2 | Hydrogenation | Benzene | H₂ | Cyclohexane |
| 3 | Bromination | Benzene | Br₂ | Bromobenzene |
| 4 | Simon-Craft | C₄H₈ | CHCl₃ | C₄H₆ |
| 5 | Friedel-Craft Alkylation | Benzene | CH₃Cl | Toluene |

---

## Features

- **CPK colouring** — standard element colour scheme for all atoms
- **Phong lighting** — smooth shading with ambient, diffuse, and specular highlights
- **Bond detection** — automatic bond drawing based on atomic covalent radii
- **Mipmapped textures** — BMP texture maps on atom spheres with trilinear filtering
- **Bohr orbit animation** — animated electron dots orbiting nuclei
- **Per-atom spin** — each atom rotates on its own axis
- **Scene auto-spin** — whole reaction rotates continuously
- **Electron shells** — translucent outer shell overlay
- **Planar shadows** — light-projected shadows cast onto the floor
- **Fog** — linear depth fog for depth cues
- **Environment map** — sphere-map reflection mode
- **Billboard atom labels** — element symbol displayed above each atom
- **Stencil-based atom picking** — click any atom to read its element info
- **Perspective / Orthographic** projection toggle
- **Right-click menu** — full toggle menu for all features
- **Status bar** — live HUD showing all currently active features
## Screenshots

### Main Reaction View
![Main Scene](screenshots/sulphonation.png)

### Interactive Menu & Controls
![Menu](screenshots/menu.png)

### Close-up Molecular Rendering
![Closeup](screenshots/closeup.png)
