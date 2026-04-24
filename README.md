# ChemReact3D — OpenGL 3D Chemical Reaction Viewer

An interactive 3D visualiser for organic chemistry reactions built with OpenGL and freeglut.  
Displays reactants and products using CPK-coloured atom spheres, bond cylinders, electron-orbit animations, and real-time lighting with floor shadows.

---

## Acknowledgement

This project was developed by using the base code from :contentReference[oaicite:0]{index=0} repository: **Opengl-3D-Chemical-Reaction-View** by **im-Amitto**.  
Original source: https://github.com/im-Amitto/Opengl-3D-Chemical-Reaction-View

We further extended and modified the original implementation by adding new features, improving visuals, enhancing interactivity, updating reactions, restructuring files, and refining the overall user experience.

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
├── screenshots/             # Screenshots of the running application
├── README.md
└── run.sh                   # Linux build & run script
---

## Dependencies

| Library | Purpose | Install Command (Ubuntu/Debian) |
|---|---|---|
| `g++` | C++ compiler | `sudo apt install build-essential` |
| `freeglut3` | OpenGL windowing & input | `sudo apt install freeglut3-dev` |
| `libGL` / `libGLU` | OpenGL / GLU rendering | included with freeglut3-dev |

---

## Installation (Linux)

### 1. Install dependencies
```bash
sudo apt update
sudo apt install -y build-essential freeglut3-dev libgl1-mesa-dev libglu1-mesa-dev
```

### 2. Clone / extract the project
```bash
unzip Group_ID_ChemReact3D.zip
cd Group_ID_ChemReact3D
```

### 3. Build
```bash
g++ ChemReact3D.cpp lib/RgbImage.cpp -o ChemReact3D \
    -lGL -lGLU -lglut -lm -std=c++17
```

### 4. Run
```bash
./ChemReact3D
```

Or simply use the provided script:
```bash
chmod +x run.sh
./run.sh
```

---

## Controls

### Mouse
| Action | Result |
|---|---|
| Left-drag | Rotate scene |
| Middle-drag | Pan scene |
| Scroll wheel | Zoom in / out |
| Left-click | Select atom / advance presentation |
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
- **Bond detection** — automatic bond drawing based on atomic radii
- **Textures** — BMP texture maps on atom spheres
- **Electron shells** — translucent outer shell overlay
- **Bohr orbit animation** — animated electron dots orbiting nuclei
- **Per-atom spin** — each atom rotates on its own axis
- **Scene auto-spin** — whole reaction rotates continuously
- **Planar shadows** — light-projected shadows cast onto the floor
- **Fog** — linear depth fog for depth cues
- **Stencil-based atom picking** — click any atom to read its element info
- **Perspective / Orthographic** projection toggle
- **Right-click menu** — full toggle menu for all features

---

## Notes

- XYZ files must use element symbol followed by X Y Z coordinates, one atom per line.
- BMP textures must be in uncompressed 24-bit RGB format.
- The application window is fixed at 1080 × 700 pixels.
