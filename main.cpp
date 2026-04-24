// Base reference:
// https://github.com/im-Amitto/Opengl-3D-Chemical-Reaction-View 
// =============================================================================
//  ChemReact3D — OpenGL 3D Chemical Reaction Viewer
// =============================================================================

#include <bits/stdc++.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include "lib/RgbImage.cpp"
#include "lib/RgbImage.h"

using namespace std;

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif
#ifndef GL_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#endif
#ifndef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#endif
#ifndef GL_GENERATE_MIPMAP_HINT
#define GL_GENERATE_MIPMAP_HINT 0x8192
#endif

// ─────────────────────────────────────────────────────────────────────────────
//  SECTION 1 — MENU / FLAG GLOBALS
// ─────────────────────────────────────────────────────────────────────────────
static int window;
static int menu_id;
static int submenu_id;
static int value = 0;

int start = 0;
int textMode = 1;
int floorMode = 1;
int presentation = 0;
float atomSize = 0.4f;
bool bondMode = true;
bool atomMode = true;

// ─────────────────────────────────────────────────────────────────────────────
//  SECTION 2 — NEW FEATURE FLAGS
// ─────────────────────────────────────────────────────────────────────────────
bool fogEnabled = true;
bool shellMode = true;
bool autoSpinActive = false;
bool envMapMode = false;
bool labelMode = true;
bool shadowMode = true;
float autoSpinAngle = 0.0f;
float autoSpinSpeed = 0.4f;

// Per-atom spin
bool atomSelfSpinActive = true;
float atomSpinAngle = 0.0f;
float atomSpinSpeed = 2.8f;

// Bohr-style orbiting electron dots
bool electronOrbitMode = true;
float electronOrbitPhase = 0.0f;

// ─────────────────────────────────────────────────────────────────────────────
//  SECTION 3 — CAMERA
// ─────────────────────────────────────────────────────────────────────────────
float camDist = 22.0f;
float camRotX = 20.0f;
float camRotY = 0.0f;
float panX = 0.0f;
float panY = 0.0f;
int lastMouseX = 0, lastMouseY = 0;
bool leftDown = false, middleDown = false;

bool perspectiveProj = true;
float orthoHalfHeight = 13.0f;

// ─────────────────────────────────────────────────────────────────────────────
//  SECTION 4 — ELEMENT / ATOM DATA
// ─────────────────────────────────────────────────────────────────────────────
enum Symbol
{
    INVALID = -1,
    H = 1,
    He,
    Li,
    Be,
    B,
    C,
    N,
    O,
    F,
    Ne,
    Na,
    Mg,
    Al,
    Si,
    P,
    S,
    Cl,
    Ar,
    K,
    Ca,
    Sc,
    Ti,
    V,
    Cr,
    Mn,
    Fe,
    Co,
    Ni,
    Cu,
    Zn,
    Ga,
    Ge,
    As,
    Se,
    Br,
    Kr,
    Rb,
    Sr,
    Y,
    Zr,
    Nb,
    Mo,
    Tc,
    Ru,
    Rh,
    Pd,
    Ag,
    Cd,
    In,
    Sn,
    Sb,
    Te,
    I,
    Xe,
    Cs,
    Ba,
    Hf = 72,
    Ta,
    W,
    Re,
    Os,
    Ir,
    Pt,
    Au,
    Hg,
    Tl,
    Pb,
    Bi,
    Po,
    At,
    Rn,
    Fr,
    Ra
};

struct CPKColor
{
    float r, g, b;
};
map<int, CPKColor> cpkColors = {
    {H, {1.00f, 1.00f, 1.00f}},
    {C, {0.25f, 0.25f, 0.25f}},
    {N, {0.05f, 0.05f, 0.90f}},
    {O, {0.90f, 0.05f, 0.05f}},
    {F, {0.50f, 0.70f, 0.10f}},
    {Cl, {0.12f, 0.94f, 0.12f}},
    {Br, {0.65f, 0.16f, 0.16f}},
    {I, {0.58f, 0.00f, 0.58f}},
    {S, {1.00f, 0.78f, 0.20f}},
    {P, {1.00f, 0.50f, 0.00f}},
    {Na, {0.67f, 0.36f, 0.95f}},
    {Mg, {0.54f, 1.00f, 0.00f}},
    {Fe, {0.88f, 0.40f, 0.20f}},
    {Ca, {0.24f, 1.00f, 0.00f}},
};

CPKColor getCPK(int sym)
{
    auto it = cpkColors.find(sym);
    if (it != cpkColors.end())
        return it->second;
    float r = 0, g = 0, b = 0;
    int index = sym % 6, row = sym / 6;
    if (index == 0 || index == 3 || index == 4)
        r = 1.0f / (row + 1);
    if (index == 1 || index == 3 || index == 5)
        g = 1.0f / (row + 1);
    if (index == 2 || index == 4 || index == 5)
        b = 1.0f / (row + 1);
    if (r + g + b < 0.1f)
    {
        r = 0.7f;
        g = 0.7f;
        b = 0.7f;
    }
    return {r, g, b};
}

map<int, float> vdwRadius = {
    {H, 1.2f}, {C, 1.7f}, {N, 1.55f}, {O, 1.52f}, {S, 1.8f}, {P, 1.8f}, {Cl, 1.75f}, {Br, 1.85f}, {I, 1.98f}};

GLuint texture[120];
string atomInfoS = "";
string Instruction =
    "Controls:\n"
    "  Left-drag   : Rotate\n"
    "  Middle-drag : Pan\n"
    "  Scroll      : Zoom (persp) / ortho scale (parallel)\n"
    "  G           : Toggle perspective / parallel projection\n"
    "  [ ]         : Narrow / widen ortho view (parallel mode)\n"
    "  SPACE       : Auto-spin (whole scene)\n"
    "  O           : Atom self-spin (on each center)\n"
    "  N / P       : Next / Prev reaction\n"
    "  F E V L H   : Fog / Shells / Env map / Labels / Shadows\n"
    "  B           : Orbiting electrons (Bohr-style dots)\n"
    "  WASD + Q/T  : Pan / zoom\n"
    "  Right-click : Menu";
string projectInfo = "ChemReact3D — 3D Chemical Reaction Exposition using OpenGL";
string AtomInfo[120];

// ─────────────────────────────────────────────────────────────────────────────
//  SECTION 5 — ATOM / MOLECULE STRUCTURES
// ─────────────────────────────────────────────────────────────────────────────
struct AtomDetail
{
    AtomDetail(Symbol s, string n, double r, double br)
        : symbol(s), name(n), radius(r), bondRadius(br) {}
    Symbol symbol;
    string name;
    double radius, bondRadius;
};

struct Atom
{
    Atom(Symbol t) : symbol(t) {}
    Symbol symbol;
    double x, y, z;
    double DistanceSquared(Atom o)
    {
        return (x - o.x) * (x - o.x) + (y - o.y) * (y - o.y) + (z - o.z) * (z - o.z);
    }
};

struct Molecule
{
    vector<Atom> atoms;
    vector<tuple<int, int>> bonds;
};

// ─────────────────────────────────────────────────────────────────────────────
//  SECTION 6 — rxn (reactant A, reactant B, product only — no intermediate)
// ─────────────────────────────────────────────────────────────────────────────
struct rxn
{
    string a, b, c, name;
};
rxn mole[5];
int moleI = 0;

// ─────────────────────────────────────────────────────────────────────────────
//  SECTION 7 — PERIODIC TABLE
// ─────────────────────────────────────────────────────────────────────────────
struct PeriodicTable
{
    PeriodicTable()
    {
        table.push_back(AtomDetail(Symbol::H, "H", 0.60, .37));
        table.push_back(AtomDetail(Symbol::He, "He", 1.90, .32));
        table.push_back(AtomDetail(Symbol::Li, "Li", 1.55, 1.34));
        table.push_back(AtomDetail(Symbol::Be, "Be", 1.12, .90));
        table.push_back(AtomDetail(Symbol::B, "B", 0.98, .82));
        table.push_back(AtomDetail(Symbol::C, "C", 0.91, .77));
        table.push_back(AtomDetail(Symbol::N, "N", 0.92, .75));
        table.push_back(AtomDetail(Symbol::O, "O", 0.80, .73));
        table.push_back(AtomDetail(Symbol::F, "F", 0.57, .71));
        table.push_back(AtomDetail(Symbol::Ne, "Ne", 0.51, .69));
        table.push_back(AtomDetail(Symbol::Na, "Na", 1.90, 1.54));
        table.push_back(AtomDetail(Symbol::Mg, "Mg", 1.60, 1.30));
        table.push_back(AtomDetail(Symbol::Al, "Al", 1.40, 1.118));
        table.push_back(AtomDetail(Symbol::Si, "Si", 1.32, 1.11));
        table.push_back(AtomDetail(Symbol::P, "P", 1.28, 1.06));
        table.push_back(AtomDetail(Symbol::S, "S", 1.60, 1.02));
        table.push_back(AtomDetail(Symbol::Cl, "Cl", 1.55, .99));
        table.push_back(AtomDetail(Symbol::Ar, "Ar", 0.88, .97));
        table.push_back(AtomDetail(Symbol::K, "K", 0.88, 1.96));
        table.push_back(AtomDetail(Symbol::Ca, "Ca", 0.88, 1.74));
        table.push_back(AtomDetail(Symbol::Sc, "Sc", 0.88, 1.44));
        table.push_back(AtomDetail(Symbol::Ti, "Ti", 0.88, 1.36));
        table.push_back(AtomDetail(Symbol::V, "V", 0.88, 1.25));
        table.push_back(AtomDetail(Symbol::Cr, "Cr", 0.88, 1.27));
        table.push_back(AtomDetail(Symbol::Mn, "Mn", 0.88, 1.39));
        table.push_back(AtomDetail(Symbol::Fe, "Fe", 0.88, 1.25));
        table.push_back(AtomDetail(Symbol::Co, "Co", 0.88, 1.26));
        table.push_back(AtomDetail(Symbol::Ni, "Ni", 0.88, 1.21));
        table.push_back(AtomDetail(Symbol::Cu, "Cu", 0.88, 1.38));
        table.push_back(AtomDetail(Symbol::Zn, "Zn", 0.88, 1.31));
        table.push_back(AtomDetail(Symbol::Ga, "Ga", 0.88, 1.26));
        table.push_back(AtomDetail(Symbol::Ge, "Ge", 0.88, 1.22));
        table.push_back(AtomDetail(Symbol::As, "As", 0.88, 1.19));
        table.push_back(AtomDetail(Symbol::Se, "Se", 0.88, 1.16));
        table.push_back(AtomDetail(Symbol::Br, "Br", 1.45, 1.14));
        table.push_back(AtomDetail(Symbol::Kr, "Kr", 0.88, 1.10));
        table.push_back(AtomDetail(Symbol::Rb, "Rb", 0.88, 2.11));
        table.push_back(AtomDetail(Symbol::Sr, "Sr", 0.88, 1.92));
        table.push_back(AtomDetail(Symbol::Y, "Y", 0.88, 1.62));
        table.push_back(AtomDetail(Symbol::Zr, "Zr", 0.88, 1.48));
        table.push_back(AtomDetail(Symbol::Nb, "Nb", 0.88, 1.37));
        table.push_back(AtomDetail(Symbol::Mo, "Mo", 0.88, 1.45));
        table.push_back(AtomDetail(Symbol::Tc, "Tc", 0.88, 1.56));
        table.push_back(AtomDetail(Symbol::Ru, "Ru", 0.88, 1.26));
        table.push_back(AtomDetail(Symbol::Rh, "Rh", 0.88, 1.35));
        table.push_back(AtomDetail(Symbol::Pd, "Pd", 0.88, 1.31));
        table.push_back(AtomDetail(Symbol::Ag, "Ag", 0.88, 1.53));
        table.push_back(AtomDetail(Symbol::Cd, "Cd", 0.88, 1.48));
        table.push_back(AtomDetail(Symbol::In, "In", 0.88, 1.44));
        table.push_back(AtomDetail(Symbol::Sn, "Sn", 0.88, 1.41));
        table.push_back(AtomDetail(Symbol::Sb, "Sb", 0.88, 1.38));
        table.push_back(AtomDetail(Symbol::Te, "Te", 0.88, 1.35));
        table.push_back(AtomDetail(Symbol::I, "I", 0.88, 1.33));
        table.push_back(AtomDetail(Symbol::Xe, "Xe", 0.88, 1.30));
        table.push_back(AtomDetail(Symbol::Cs, "Cs", 0.88, 2.25));
        table.push_back(AtomDetail(Symbol::Ba, "Ba", 0.88, 1.98));
        table.push_back(AtomDetail(Symbol::Hf, "Hf", 0.88, 1.69));
        table.push_back(AtomDetail(Symbol::Ta, "Ta", 0.88, 1.74));
        table.push_back(AtomDetail(Symbol::W, "W", 0.88, 1.00));
        table.push_back(AtomDetail(Symbol::Os, "Os", 0.88, 1.00));
        table.push_back(AtomDetail(Symbol::Ir, "Ir", 0.88, 1.00));
        table.push_back(AtomDetail(Symbol::Pt, "Pt", 0.88, 1.00));
        table.push_back(AtomDetail(Symbol::Au, "Au", 0.88, 1.00));
        table.push_back(AtomDetail(Symbol::Hg, "Hg", 0.88, 1.00));
        table.push_back(AtomDetail(Symbol::Pb, "Pb", 0.88, 1.00));
        table.push_back(AtomDetail(Symbol::Bi, "Bi", 0.88, 1.74));
        table.push_back(AtomDetail(Symbol::Po, "Po", 0.88, 1.74));
        table.push_back(AtomDetail(Symbol::At, "At", 0.88, 1.74));
        table.push_back(AtomDetail(Symbol::Rn, "Rn", 0.88, 1.74));
        table.push_back(AtomDetail(Symbol::Fr, "Fr", 0.88, 1.74));
        table.push_back(AtomDetail(Symbol::Ra, "Ra", 0.88, 1.74));
    }

    bool AtomsCanBond(Atom a1, Atom a2)
    {
        double dist = a1.DistanceSquared(a2);
        auto i1 = find_if(table.cbegin(), table.cend(), [&a1](AtomDetail x)
                          { return x.symbol == a1.symbol; });
        auto i2 = find_if(table.cbegin(), table.cend(), [&a2](AtomDetail x)
                          { return x.symbol == a2.symbol; });
        if (i1 == table.cend() || i2 == table.cend())
            return false;
        double r = ((*i1).bondRadius + (*i2).bondRadius);
        return dist <= r * r * 1.1;
    }
    vector<AtomDetail> table;
};

// ─────────────────────────────────────────────────────────────────────────────
//  SECTION 8 — DATA PARSER
// ─────────────────────────────────────────────────────────────────────────────
struct DataParser
{
    DataParser(PeriodicTable t) : chem(t) {}

    Molecule ParseData(istream &stream)
    {
        Molecule mol;
        string line;
        char atomName[64];
        while (getline(stream, line))
        {
            int count;
            double x, y, z;
            if (sscanf(line.c_str(), "%s %lf %lf %lf", atomName, &x, &y, &z) == 4)
            {
                auto it = find_if(chem.table.cbegin(), chem.table.cend(),
                                  [&atomName](AtomDetail x)
                                  { return x.name == atomName; });
                if (it == chem.table.cend())
                    continue;
                Atom a((*it).symbol);
                a.x = x;
                a.y = y;
                a.z = z;
                mol.atoms.push_back(a);
            }
            else if (sscanf(line.c_str(), "%i", &count) == 1 && count > 0)
            {
                mol.atoms.reserve(count);
            }
        }
        for (auto it = mol.atoms.begin(); it != mol.atoms.end(); it++)
        {
            for (auto j = mol.atoms.begin(); j != mol.atoms.end(); j++)
            {
                if (it != j && chem.AtomsCanBond(*it, *j))
                    mol.bonds.push_back(make_tuple(it - mol.atoms.begin(), j - mol.atoms.begin()));
            }
        }
        return mol;
    }

    Molecule ParseData(string file)
    {
        ifstream s(file);
        return ParseData(s);
    }

    PeriodicTable chem;
};

PeriodicTable chem;
DataParser parser(chem);

// ─────────────────────────────────────────────────────────────────────────────
//  SECTION 9 — FLOOR
//  Lowered to Y = -5.0 so molecules (centred near Y=0) never clip through it.
// ─────────────────────────────────────────────────────────────────────────────
float fVert[4][3] = {
    {-50.0f, -5.0f, -50.0f},
    {+50.0f, -5.0f, -50.0f},
    {+50.0f, -5.0f, +50.0f},
    {-50.0f, -5.0f, +50.0f}};

// ─────────────────────────────────────────────────────────────────────────────
//  SECTION 10 — TEXTURE LOADER
// ─────────────────────────────────────────────────────────────────────────────
GLuint loadTextureFromFile(const char *filename)
{
    GLuint temp = 0;
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    RgbImage theTexMap(filename);
    glGenTextures(1, &temp);
    glBindTexture(GL_TEXTURE_2D, temp);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB,
                      theTexMap.GetNumCols(), theTexMap.GetNumRows(),
                      GL_RGB, GL_UNSIGNED_BYTE, theTexMap.ImageData());

    const char *ext = (const char *)glGetString(GL_EXTENSIONS);
    if (ext && strstr(ext, "EXT_texture_filter_anisotropic"))
    {
        GLfloat maxA = 1.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxA);
        if (maxA > 1.0f)
        {
            GLfloat use = (maxA > 8.0f) ? 8.0f : maxA;
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, use);
        }
    }
    return temp;
}

// ─────────────────────────────────────────────────────────────────────────────
//  SECTION 11 — LIGHTING
//  Light placed high directly above (Y=20) so shadows fall clearly downward.
// ─────────────────────────────────────────────────────────────────────────────
void setupLighting()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glDisable(GL_LIGHT1);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    GLfloat globalAmb[] = {0.30f, 0.30f, 0.30f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    // High point light directly above — casts clear downward shadows
    GLfloat pos[] = {0.0f, 20.0f, 2.0f, 1.0f};

    GLfloat keyAmb[] = {0.25f, 0.22f, 0.08f, 1.0f};
    GLfloat keyDiff[] = {1.80f, 1.60f, 0.80f, 1.0f};
    GLfloat keySpec[] = {1.0f, 1.0f, 0.80f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, keyAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, keyDiff);
    glLightfv(GL_LIGHT0, GL_SPECULAR, keySpec);
}

void setAtomMaterial(float r, float g, float b, float alpha = 1.0f)
{
    GLfloat diff[] = {r, g, b, alpha};
    GLfloat spec[] = {0.9f, 0.9f, 0.9f, alpha};
    GLfloat amb[] = {r * 0.3f, g * 0.3f, b * 0.3f, alpha};
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 80.0f);
}

// ─────────────────────────────────────────────────────────────────────────────
//  SECTION 12 — FOG
// ─────────────────────────────────────────────────────────────────────────────
void setupFog()
{
    if (!fogEnabled)
    {
        glDisable(GL_FOG);
        return;
    }
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, 15.0f);
    glFogf(GL_FOG_END, 60.0f);
    GLfloat fc[] = {0.05f, 0.05f, 0.10f, 1.0f};
    glFogfv(GL_FOG_COLOR, fc);
    glHint(GL_FOG_HINT, GL_NICEST);
}

// ─────────────────────────────────────────────────────────────────────────────
//  SECTION 13 — CYLINDER
// ─────────────────────────────────────────────────────────────────────────────
void renderCylinder(float x1, float y1, float z1,
                    float x2, float y2, float z2,
                    float radius, int sub, GLUquadricObj *q)
{
    float vx = x2 - x1, vy = y2 - y1, vz = z2 - z1;
    if (vz == 0)
        vz = .0001f;
    float v = sqrt(vx * vx + vy * vy + vz * vz);
    float ax = 57.2957795f * acos(vz / v);
    if (vz < 0)
        ax = -ax;
    float rx = -vy * vz, ry = vx * vz;
    glPushMatrix();
    glTranslatef(x1, y1, z1);
    glRotatef(ax, rx, ry, 0);
    gluQuadricOrientation(q, GLU_OUTSIDE);
    gluCylinder(q, radius, radius, v, sub, 1);
    gluQuadricOrientation(q, GLU_INSIDE);
    gluDisk(q, 0, radius, sub, 1);
    glTranslatef(0, 0, v);
    gluQuadricOrientation(q, GLU_OUTSIDE);
    gluDisk(q, 0, radius, sub, 1);
    glPopMatrix();
}

// ─────────────────────────────────────────────────────────────────────────────
//  SECTION 14 — TEXT HELPERS
// ─────────────────────────────────────────────────────────────────────────────
void stringPrint(const string &s, int x, int y, float r, float g, float b)
{
    void *font = GLUT_BITMAP_9_BY_15;
    int qp = 0;
    glRasterPos2i(x, y);
    for (char c : s)
    {
        if (c == '\n')
        {
            qp += 20;
            glRasterPos2i(x, y - qp);
        }
        else
        {
            glColor3f(r, g, b);
            glutBitmapCharacter(font, c);
        }
    }
}
void stringPrinter(const string &s, int x, int y, float r, float g, float b)
{
    if (textMode)
        stringPrint(s, x, y, r, g, b);
}

// ─────────────────────────────────────────────────────────────────────────────
//  SECTION 15 — BILLBOARD LABEL
// ─────────────────────────────────────────────────────────────────────────────
void drawBillboardLabel(float wx, float wy, float wz, const string &text,
                        float r, float g, float b)
{
    if (!labelMode || !textMode)
        return;
    float mv[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, mv);
    float rx = mv[0], ry = mv[4], rz = mv[8];
    float off = 0.6f;
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glColor3f(r, g, b);
    glRasterPos3f(wx + rx * off, wy + ry * off, wz + rz * off);
    for (char c : text)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

// ─────────────────────────────────────────────────────────────────────────────
//  SECTION 16 — ATOM DRAWING
// ─────────────────────────────────────────────────────────────────────────────
void drawAtom(float ox, float oy, float oz, float radius, int symInt,
              bool isSelected)
{
    CPKColor col = getCPK(symInt);
    if (isSelected)
    {
        col.r = 1.0f;
        col.g = 1.0f;
        col.b = 0.0f;
    }

    glPushMatrix();
    glTranslatef(ox, oy, oz);
    if (atomSelfSpinActive)
    {
        float spin = atomSpinAngle + (float)(symInt % 13) * 23.0f;
        glRotatef(spin, 0.0f, 1.0f, 0.3f);
    }

    setAtomMaterial(col.r, col.g, col.b);

    GLUquadric *q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);

    if (texture[symInt] && !envMapMode)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture[symInt]);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        gluQuadricTexture(q, GL_TRUE);
    }
    else if (envMapMode && texture[symInt])
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture[symInt]);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    }

    gluSphere(q, radius, 40, 40);

    glDisable(GL_TEXTURE_2D);
    if (envMapMode)
    {
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
    }
    gluDeleteQuadric(q);

    if (shellMode)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);

        GLfloat sc[] = {col.r, col.g, col.b, 0.15f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, sc);
        GLfloat ss[] = {col.r, col.g, col.b, 0.15f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ss);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0f);

        GLUquadric *qs = gluNewQuadric();
        gluQuadricNormals(qs, GLU_SMOOTH);
        gluSphere(qs, radius * 1.45f, 24, 24);
        gluDeleteQuadric(qs);

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        setAtomMaterial(col.r, col.g, col.b);
    }

    glPopMatrix();
}

static int orbitElectronCount(int symInt)
{
    if (symInt <= 0)
        return 1;
    return symInt < 8 ? symInt : 8;
}

void drawOrbitingElectrons(float cx, float cy, float cz, float atomR, int symInt)
{
    if (!electronOrbitMode || atomR <= 1e-5f)
        return;

    const float d2r = 3.14159265f / 180.0f;
    const float twopi = 6.2831853f;
    int n = orbitElectronCount(symInt);
    float eRad = fmaxf(atomR * 0.11f, 0.034f);
    float seed = fmodf(cx * 7.31f + cy * 5.17f + cz * 4.73f + (float)symInt * 23.0f, twopi);

    glStencilFunc(GL_ALWAYS, 0, -1);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < n; ++i)
    {
        int ring = i % 3;
        float orbitR = atomR * (1.22f + 0.26f * ring);
        float spd = 1.0f + 0.11f * (float)(i % 4);
        float ang = electronOrbitPhase * spd * d2r + seed + twopi * (float)i / (float)n;
        float co = cosf(ang), si = sinf(ang);

        glPushMatrix();
        glTranslatef(cx, cy, cz);
        if (ring == 0)
        {
            glTranslatef(co * orbitR, 0.0f, si * orbitR);
        }
        else if (ring == 1)
        {
            glRotatef(64.0f, 1.0f, 0.0f, 0.0f);
            glTranslatef(co * orbitR, 0.0f, si * orbitR);
        }
        else
        {
            glRotatef(58.0f, 0.0f, 1.0f, 0.0f);
            glTranslatef(co * orbitR, 0.0f, si * orbitR);
        }
        glColor4f(0.2f, 0.88f, 1.0f, 0.9f);
        glutSolidSphere(eRad, 10, 10);
        glPopMatrix();
    }

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// ─────────────────────────────────────────────────────────────────────────────
//  SECTION 17 — PLANAR SHADOW
//  floorY matches the lowered floor (-5.0).  Alpha raised to 0.35 for
//  contrast.  Shadow position uses actual light position for correct offset.
// ─────────────────────────────────────────────────────────────────────────────
void drawShadow(float ox, float oy, float oz, float radius)
{
    if (!shadowMode || !floorMode)
        return;

    const float floorY = -5.0f;                   // matches lowered fVert
    const float lx = 0.0f, ly = 20.0f, lz = 2.0f; // matches GL_LIGHT0 pos

    // Skip atoms that are already below the floor
    if (oy <= floorY)
        return;

    // Project atom centre onto floor plane along the light ray
    float t = (floorY - ly) / (oy - ly);
    float sx = lx + t * (ox - lx);
    float sz = lz + t * (oz - lz);

    glPushMatrix();
    glTranslatef(sx, floorY + 0.01f, sz);
    glScalef(1.0f, 0.001f, 1.0f);
    glDisable(GL_LIGHTING);
    glColor4f(0.0f, 0.0f, 0.0f, 0.35f); // more opaque for visibility
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glutSolidSphere(radius * atomSize * 1.2f, 16, 16);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

// ─────────────────────────────────────────────────────────────────────────────
//  SECTION 18 — MAIN DRAW REACTION
//  molecule[0] = reactant A,  molecule[1] = reactant B,  molecule[2] = product
//  A Y-offset of +1.5 lifts the entire molecule group above the floor so no
//  atom clips through the floor plane.
// ─────────────────────────────────────────────────────────────────────────────
int selectedAtomSymbol = 0;
static const float MOL_Y_OFFSET = 1.5f; // lifts molecules above the floor

void drawReaction()
{
    Molecule molecule[3] = {
        parser.ParseData(mole[moleI].a),
        parser.ParseData(mole[moleI].b),
        parser.ParseData(mole[moleI].c)};

    int pk = presentation;

    for (int i = 0, k = -1; i < 3; i++, k++)
    {

        if (moleI > 4 && presentation != -2)
        {
            presentation = -2;
            glutPostRedisplay();
        }

        if (pk == 0)
        {
            stringPrinter(projectInfo, 400, 450, 0.0f, 1.0f, 1.0f);
            return;
        }

        if (pk == -1 || pk > 0)
        {

            if (shadowMode && floorMode)
            {
                for (auto &atom : molecule[i].atoms)
                {
                    auto it1 = find_if(chem.table.cbegin(), chem.table.cend(),
                                       [&atom](AtomDetail x)
                                       { return x.symbol == atom.symbol; });
                    float r = (it1 != chem.table.cend()) ? (float)(*it1).radius : 0.5f;
                    float ax = (float)atom.x - (10.0f * k);
                    float ay = (float)atom.y + MOL_Y_OFFSET;
                    float az = (float)atom.z;
                    drawShadow(ax, ay, az, r);
                }
            }

            if (atomMode)
            {
                for (auto &atom : molecule[i].atoms)
                {
                    auto it1 = find_if(chem.table.cbegin(), chem.table.cend(),
                                       [&atom](AtomDetail x)
                                       { return x.symbol == atom.symbol; });
                    float radius = (it1 != chem.table.cend()) ? (float)(*it1).radius : 0.5f;

                    float ax = (float)atom.x - (10.0f * k);
                    float ay = (float)atom.y + MOL_Y_OFFSET;
                    float az = (float)atom.z;

                    glStencilFunc(GL_ALWAYS, atom.symbol, -1);

                    bool selected = (atom.symbol == selectedAtomSymbol && selectedAtomSymbol != 0);
                    drawAtom(ax, ay, az, radius * atomSize, (int)atom.symbol, selected);
                    drawOrbitingElectrons(ax, ay, az, radius * atomSize, (int)atom.symbol);

                    CPKColor col = getCPK((int)atom.symbol);
                    string symName = "?";
                    if (it1 != chem.table.cend())
                        symName = (*it1).name;
                    drawBillboardLabel(ax, ay + radius * atomSize + 0.3f, az, symName,
                                       col.r > 0.7f ? 0.0f : 1.0f,
                                       1.0f,
                                       col.b > 0.7f ? 0.0f : 1.0f);
                }
            }

            if (bondMode)
            {
                glDisable(GL_TEXTURE_2D);
                GLfloat bd[] = {0.8f, 0.85f, 0.7f, 1.0f};
                GLfloat bs[] = {1.0f, 1.0f, 1.0f, 1.0f};
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, bd);
                glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, bs);
                glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0f);

                glStencilFunc(GL_ALWAYS, 0, -1);
                for (auto &bond : molecule[i].bonds)
                {
                    Atom a1 = molecule[i].atoms[get<0>(bond)];
                    Atom a2 = molecule[i].atoms[get<1>(bond)];

                    float ax1 = (float)a1.x - (10.0f * k), ay1 = (float)a1.y + MOL_Y_OFFSET, az1 = (float)a1.z;
                    float ax2 = (float)a2.x - (10.0f * k), ay2 = (float)a2.y + MOL_Y_OFFSET, az2 = (float)a2.z;

                    GLUquadricObj *q = gluNewQuadric();
                    gluQuadricNormals(q, GLU_SMOOTH);
                    renderCylinder(ax1, ay1, az1, ax2, ay2, az2, 0.08f, 20, q);
                    gluDeleteQuadric(q);
                }
            }

            // Arrow between molecules (also offset by MOL_Y_OFFSET)
            if (i == 0)
            {
                glDisable(GL_LIGHTING);
                glColor3f(0.0f, 1.0f, 0.0f);
                GLUquadricObj *q = gluNewQuadric();
                gluQuadricNormals(q, GLU_SMOOTH);
                renderCylinder(4, MOL_Y_OFFSET, 0, 5, MOL_Y_OFFSET, 0, 0.12f, 20, q);
                gluDeleteQuadric(q);
                q = gluNewQuadric();
                gluQuadricNormals(q, GLU_SMOOTH);
                renderCylinder(4.5f, MOL_Y_OFFSET - 0.5f, 0, 4.5f, MOL_Y_OFFSET + 0.5f, 0, 0.12f, 20, q);
                gluDeleteQuadric(q);
                glEnable(GL_LIGHTING);
            }
            else if (i == 1)
            {
                glDisable(GL_LIGHTING);
                glColor3f(1.0f, 0.0f, 0.0f);
                GLUquadricObj *q = gluNewQuadric();
                gluQuadricNormals(q, GLU_SMOOTH);
                renderCylinder(-4.5f, MOL_Y_OFFSET + 0.25f, 0, -5.5f, MOL_Y_OFFSET + 0.25f, 0, 0.12f, 20, q);
                gluDeleteQuadric(q);
                q = gluNewQuadric();
                gluQuadricNormals(q, GLU_SMOOTH);
                renderCylinder(-4.5f, MOL_Y_OFFSET - 0.25f, 0, -5.5f, MOL_Y_OFFSET - 0.25f, 0, 0.12f, 20, q);
                gluDeleteQuadric(q);
                glEnable(GL_LIGHTING);
            }
        }

        if (pk != -1)
            pk--;
        if (pk == 1 && k == 1)
        {
            presentation = 1;
            moleI++;
            glutPostRedisplay();
        }
    }
}

void drawFloor()
{
    if (!floorMode)
        return;

    glDisable(GL_LIGHTING);
    glColor4f(0.28f, 0.28f, 0.32f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3fv(fVert[0]);
    glVertex3fv(fVert[1]);
    glVertex3fv(fVert[2]);
    glVertex3fv(fVert[3]);
    glEnd();
    glEnable(GL_LIGHTING);
}

void menu(int num)
{
    if (num == 0)
    {
        glutDestroyWindow(window);
        exit(0);
    }
    else
        value = num;
    glutPostRedisplay();
}

void createMenu()
{
    submenu_id = glutCreateMenu(menu);
    glutAddMenuEntry("Increase", 40);
    glutAddMenuEntry("Decrease", 41);

    menu_id = glutCreateMenu(menu);
    glutAddMenuEntry(atomMode ? "Atoms - Off" : "Atoms - On", atomMode ? 20 : 21);
    glutAddMenuEntry(bondMode ? "Bonds - Off" : "Bonds - On", bondMode ? 30 : 31);
    glutAddMenuEntry(shellMode ? "Shells - Off" : "Shells - On", shellMode ? 80 : 81);
    glutAddMenuEntry(fogEnabled ? "Fog - Off" : "Fog - On", fogEnabled ? 82 : 83);
    glutAddMenuEntry(autoSpinActive ? "Auto-Spin - Off" : "Auto-Spin - On", autoSpinActive ? 84 : 85);
    glutAddMenuEntry(atomSelfSpinActive ? "Atom spin - Off" : "Atom spin - On", atomSelfSpinActive ? 92 : 93);
    glutAddMenuEntry(perspectiveProj ? "Projection: Parallel" : "Projection: Perspective", perspectiveProj ? 96 : 97);
    glutAddMenuEntry(electronOrbitMode ? "Orbit e- - Off" : "Orbit e- - On", electronOrbitMode ? 100 : 101);
    glutAddMenuEntry(labelMode ? "Labels - Off" : "Labels - On", labelMode ? 86 : 87);
    glutAddMenuEntry(shadowMode ? "Shadows - Off" : "Shadows - On", shadowMode ? 88 : 89);
    glutAddMenuEntry(envMapMode ? "Env Map - Off" : "Env Map - On", envMapMode ? 90 : 91);
    glutAddMenuEntry(presentation != -1 ? "Presentation - Off" : "Presentation - On", presentation != -1 ? 50 : 51);
    glutAddMenuEntry(textMode ? "Info - Off" : "Info - On", textMode ? 60 : 61);
    glutAddMenuEntry(floorMode ? "Floor - Off" : "Floor - On", floorMode ? 70 : 71);
    glutAddSubMenu("Atom Size", submenu_id);
    glutAddMenuEntry("Quit", 0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void display()
{
    switch (value)
    {
    case 20:
        atomMode = false;
        value = 0;
        createMenu();
        break;
    case 21:
        atomMode = true;
        value = 0;
        createMenu();
        break;
    case 30:
        bondMode = false;
        value = 0;
        createMenu();
        break;
    case 31:
        bondMode = true;
        value = 0;
        createMenu();
        break;
    case 40:
        atomSize += 0.1f;
        value = 0;
        break;
    case 41:
        atomSize -= 0.1f;
        value = 0;
        break;
    case 50:
        presentation = -1;
        moleI = 0;
        value = 0;
        createMenu();
        break;
    case 51:
        presentation = 0;
        moleI = 0;
        value = 0;
        createMenu();
        break;
    case 60:
        textMode = 0;
        value = 0;
        createMenu();
        break;
    case 61:
        textMode = 1;
        value = 0;
        createMenu();
        break;
    case 70:
        floorMode = 0;
        value = 0;
        createMenu();
        break;
    case 71:
        floorMode = 1;
        value = 0;
        createMenu();
        break;
    case 80:
        shellMode = false;
        value = 0;
        createMenu();
        break;
    case 81:
        shellMode = true;
        value = 0;
        createMenu();
        break;
    case 82:
        fogEnabled = false;
        setupFog();
        value = 0;
        createMenu();
        break;
    case 83:
        fogEnabled = true;
        setupFog();
        value = 0;
        createMenu();
        break;
    case 84:
        autoSpinActive = false;
        value = 0;
        createMenu();
        break;
    case 85:
        autoSpinActive = true;
        value = 0;
        createMenu();
        break;
    case 86:
        labelMode = false;
        value = 0;
        createMenu();
        break;
    case 87:
        labelMode = true;
        value = 0;
        createMenu();
        break;
    case 88:
        shadowMode = false;
        value = 0;
        createMenu();
        break;
    case 89:
        shadowMode = true;
        value = 0;
        createMenu();
        break;
    case 90:
        envMapMode = false;
        value = 0;
        createMenu();
        break;
    case 91:
        envMapMode = true;
        value = 0;
        createMenu();
        break;
    case 92:
        atomSelfSpinActive = false;
        value = 0;
        createMenu();
        break;
    case 93:
        atomSelfSpinActive = true;
        value = 0;
        createMenu();
        break;
    case 96:
        perspectiveProj = false;
        value = 0;
        createMenu();
        glutReshapeWindow(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        break;
    case 97:
        perspectiveProj = true;
        value = 0;
        createMenu();
        glutReshapeWindow(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        break;
    case 100:
        electronOrbitMode = false;
        value = 0;
        createMenu();
        break;
    case 101:
        electronOrbitMode = true;
        value = 0;
        createMenu();
        break;
    }

    glClearColor(0.05f, 0.05f, 0.10f, 1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glLoadIdentity();
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    setupLighting();
    setupFog();

    if (presentation == 0 && moleI == 0)
    {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, 1080, 0, 700);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glDisable(GL_LIGHTING);
        glDisable(GL_FOG);
        glDisable(GL_DEPTH_TEST);
        stringPrinter(projectInfo, 400, 450, 0.0f, 1.0f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        if (fogEnabled)
            glEnable(GL_FOG);
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glutSwapBuffers();
        return;
    }

    glTranslatef(panX, panY, -camDist);
    glRotatef(camRotX, 1, 0, 0);
    glRotatef(camRotY, 0, 1, 0);
    glRotatef(autoSpinAngle, 0, 1, 0);

    glPushMatrix();
    drawFloor();
    glPopMatrix();

    glPushMatrix();
    drawReaction();
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1080, 0, 700);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glDisable(GL_DEPTH_TEST);

    if (presentation != -2 && (moleI > 0 || presentation != 0))
    {
        string rname = (presentation == 0) ? mole[moleI - 1].name : mole[moleI].name;
        stringPrinter(rname, 850, 650, 1.0f, 0.0f, 1.0f);
        stringPrinter(atomInfoS, 10, 130, 0.0f, 1.0f, 0.0f);
        stringPrinter(projectInfo, 10, 660, 0.0f, 1.0f, 1.0f);
        stringPrinter(Instruction, 700, 120, 1.0f, 1.0f, 0.0f);
    }
    if (presentation == -2)
        stringPrinter("Presentation finished.\nClick to exit.", 430, 450, 0.0f, 1.0f, 1.0f);

    string status = "";
    if (autoSpinActive)
        status += "[SCENE-SPIN] ";
    if (atomSelfSpinActive)
        status += "[ATOM-SPIN] ";
    status += perspectiveProj ? "[PERSP] " : "[ORTHO] ";
    if (electronOrbitMode)
        status += "[e- ORBIT] ";
    if (fogEnabled)
        status += "[FOG] ";
    if (shellMode)
        status += "[SHELLS] ";
    if (envMapMode)
        status += "[ENVMAP] ";
    if (shadowMode)
        status += "[SHADOWS] ";
    stringPrinter(status, 10, 20, 0.6f, 0.6f, 0.6f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    if (fogEnabled)
        glEnable(GL_FOG);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glutSwapBuffers();
}

void mouseFunc(int button, int state, int x, int y)
{

    // Very first click ever: leave title screen and show first reaction directly
    if (state == GLUT_DOWN && start == 0)
    {
        presentation = -1;
        start = -1;
        createMenu();
        glutPostRedisplay();
        return;
    }

    lastMouseX = x;
    lastMouseY = y;

    if (button == GLUT_LEFT_BUTTON)
    {
        leftDown = (state == GLUT_DOWN);
        if (state == GLUT_DOWN)
            autoSpinActive = false;
    }
    if (button == GLUT_MIDDLE_BUTTON)
        middleDown = (state == GLUT_DOWN);

    if (button == 3)
    {
        if (perspectiveProj)
        {
            camDist -= 0.8f;
            if (camDist < 1.0f)
                camDist = 1.0f;
        }
        else
        {
            orthoHalfHeight *= 0.92f;
            if (orthoHalfHeight < 3.0f)
                orthoHalfHeight = 3.0f;
            glutReshapeWindow(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        }
    }
    if (button == 4)
    {
        if (perspectiveProj)
        {
            camDist += 0.8f;
        }
        else
        {
            orthoHalfHeight *= 1.08f;
            if (orthoHalfHeight > 80.0f)
                orthoHalfHeight = 80.0f;
            glutReshapeWindow(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        }
    }

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && start != 0)
    {
        int wh = glutGet(GLUT_WINDOW_HEIGHT);
        GLuint index;
        glReadPixels(x, wh - y - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);
        selectedAtomSymbol = (int)index;
        atomInfoS = "Atomic Number: " + to_string(index) + "\n" + AtomInfo[index];

        // Always jump to full reaction view on click (no step-by-step)
        if (presentation == -2)
        {
            // After "Presentation finished" — reset to first reaction
            presentation = -1;
            moleI = 0;
            createMenu();
        }
        else if (presentation != -1)
        {
            // Still in title/step mode — jump straight to full view
            presentation = -1;
            moleI = 0;
            createMenu();
        }
    }

    glutPostRedisplay();
}

void mouseMotion(int x, int y)
{
    int dx = x - lastMouseX, dy = y - lastMouseY;
    if (leftDown)
    {
        camRotY += dx * 0.5f;
        camRotX += dy * 0.5f;
    }
    if (middleDown)
    {
        panX += dx * 0.01f;
        panY -= dy * 0.01f;
    }
    lastMouseX = x;
    lastMouseY = y;
    glutPostRedisplay();
}

// ─────────────────────────────────────────────────────────────────────────────
//  SECTION 23 — TIMER
// ─────────────────────────────────────────────────────────────────────────────
void timerCB(int)
{
    if (autoSpinActive)
    {
        autoSpinAngle += autoSpinSpeed;
        if (autoSpinAngle >= 360.0f)
            autoSpinAngle -= 360.0f;
        glutPostRedisplay();
    }
    if (atomSelfSpinActive)
    {
        atomSpinAngle += atomSpinSpeed;
        if (atomSpinAngle >= 360.0f)
            atomSpinAngle -= 360.0f;
        glutPostRedisplay();
    }
    if (electronOrbitMode)
    {
        electronOrbitPhase += 1.65f;
        if (electronOrbitPhase >= 360.0f)
            electronOrbitPhase -= 360.0f;
        glutPostRedisplay();
    }
    glutTimerFunc(16, timerCB, 0);
}

// ─────────────────────────────────────────────────────────────────────────────
//  SECTION 24 — KEYBOARD
// ─────────────────────────────────────────────────────────────────────────────
void keyboardFunc(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'w':
        panY += 0.5f;
        break;
    case 's':
        panY -= 0.5f;
        break;
    case 'a':
        panX -= 0.5f;
        break;
    case 'd':
        panX += 0.5f;
        break;
    case 'q':
        camDist -= 0.5f;
        if (camDist < 1.0f)
            camDist = 1.0f;
        break;
    case 't':
        camDist += 0.5f;
        break;

    case 'n':
        moleI = (moleI + 1) % 5;
        break;
    case 'p':
        moleI = (moleI + 4) % 5;
        break;

    case ' ':
        autoSpinActive = !autoSpinActive;
        createMenu();
        break;
    case 'f':
        fogEnabled = !fogEnabled;
        setupFog();
        createMenu();
        break;
    case 'e':
        shellMode = !shellMode;
        createMenu();
        break;
    case 'v':
        envMapMode = !envMapMode;
        createMenu();
        break;
    case 'l':
        labelMode = !labelMode;
        createMenu();
        break;
    case 'h':
        shadowMode = !shadowMode;
        createMenu();
        break;
    case 'b':
    case 'B':
        electronOrbitMode = !electronOrbitMode;
        createMenu();
        break;
    case 'o':
    case 'O':
        atomSelfSpinActive = !atomSelfSpinActive;
        createMenu();
        break;
    case 'g':
    case 'G':
        perspectiveProj = !perspectiveProj;
        createMenu();
        glutReshapeWindow(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        break;
    case '[':
        if (!perspectiveProj)
        {
            orthoHalfHeight *= 1.08f;
            if (orthoHalfHeight > 80.0f)
                orthoHalfHeight = 80.0f;
            glutReshapeWindow(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        }
        break;
    case ']':
        if (!perspectiveProj)
        {
            orthoHalfHeight *= 0.92f;
            if (orthoHalfHeight < 3.0f)
                orthoHalfHeight = 3.0f;
            glutReshapeWindow(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        }
        break;

    case '+':
        autoSpinSpeed += 0.1f;
        atomSpinSpeed += 0.2f;
        break;
    case '-':
        autoSpinSpeed = max(0.0f, autoSpinSpeed - 0.1f);
        atomSpinSpeed = max(0.0f, atomSpinSpeed - 0.2f);
        break;

    case 27:
        exit(0);
    }
    glutPostRedisplay();
}

void specialKeyboard(int key, int, int)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        camDist -= 0.8f;
        if (camDist < 1.0f)
            camDist = 1.0f;
        break;
    case GLUT_KEY_DOWN:
        camDist += 0.8f;
        break;
    case GLUT_KEY_LEFT:
        camRotY -= 5.0f;
        break;
    case GLUT_KEY_RIGHT:
        camRotY += 5.0f;
        break;
    }
    glutPostRedisplay();
}

// ─────────────────────────────────────────────────────────────────────────────
//  SECTION 25 — RESHAPE / PROJECTION
// ─────────────────────────────────────────────────────────────────────────────
void applyProjection(int w, int h)
{
    if (h <= 0)
        h = 1;
    float aspect = (float)w / (float)h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (perspectiveProj)
    {
        gluPerspective(45.0, aspect, 0.1, 200.0);
    }
    else
    {
        float hh = orthoHalfHeight;
        float hw = hh * aspect;
        glOrtho(-hw, hw, -hh, hh, 0.5, 250.0);
    }
    glMatrixMode(GL_MODELVIEW);
}

void reshape(int w, int h)
{
    if (h == 0)
        h = 1;
    glViewport(0, 0, w, h);
    applyProjection(w, h);
}

// ─────────────────────────────────────────────────────────────────────────────
//  SECTION 26 — ATOM INFO
// ─────────────────────────────────────────────────────────────────────────────
void detail()
{
    AtomInfo[1] = "Hydrogen:\n1. Colourless, odourless, tasteless gas.\n2. Lightest known gas.\n3. Very slightly soluble in water.";
    AtomInfo[6] = "Carbon:\n1. Non-metallic element.\n2. Occurs free (diamond, coal, graphite).\n3. Forms hydrocarbons.";
    AtomInfo[7] = "Nitrogen:\n1. Typical non-metal, diatomic.\n2. Highly electronegative.\n3. Oxidation state -3 to +5.";
    AtomInfo[8] = "Oxygen:\n1. Non-metal, paramagnetic.\n2. Most abundant element in Earth's crust.\n3. Di-atomic (dioxygen).";
    AtomInfo[16] = "Sulphur:\n1. Used as disinfectant.\n2. Vulcanisation of rubber.\n3. Manufacture of matches and fireworks.";
    AtomInfo[17] = "Chlorine:\n1. Non-metallic element.\n2. Used in water sterilisation.\n3. Used in manufacture of PVC.";
    AtomInfo[35] = "Bromine:\n1. Reddish-brown liquid.\n2. Atomic radius 101.4 pm.\n3. Used in medicine and organic chemistry.";
}

// ─────────────────────────────────────────────────────────────────────────────
//  SECTION 27 — REACTIONS
// ─────────────────────────────────────────────────────────────────────────────
void getRxns()
{
    mole[0] = {"./XYZ/benzene.xyz", "./XYZ/h2so4.xyz", "./XYZ/benzenesulfonic_acid.xyz", "Sulphonation Reaction"};
    mole[1] = {"./XYZ/benzene.xyz", "./XYZ/hydrogen.xyz", "./XYZ/cyclohexane.xyz", "Hydrogenation Reaction"};
    mole[2] = {"./XYZ/benzene.xyz", "./XYZ/bromine.xyz", "./XYZ/bromobenzene.xyz", "Bromination Reaction"};
    mole[3] = {"./XYZ/c4h8.xyz", "./XYZ/chcl3.xyz", "./XYZ/c4h6.xyz", "Simon-Craft Reaction"};
    mole[4] = {"./XYZ/benzene.xyz", "./XYZ/ch3cl.xyz", "./XYZ/toulene.xyz", "Friedel-Craft Alkylation"};
}

// ─────────────────────────────────────────────────────────────────────────────
//  SECTION 28 — MAIN
// ─────────────────────────────────────────────────────────────────────────────
int main(int argc, char **argv)
{
    getRxns();
    detail();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL | GLUT_MULTISAMPLE);
    glutInitWindowSize(1080, 700);
    glutInitWindowPosition(50, 50);
    window = glutCreateWindow("ChemReact3D — 3D Chemical Reaction Exposition using OpenGL");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    texture[1] = loadTextureFromFile("./bmp/hydrogen.bmp");
    texture[6] = loadTextureFromFile("./bmp/carbon.bmp");
    texture[7] = loadTextureFromFile("./bmp/nitrogen.bmp");
    texture[8] = loadTextureFromFile("./bmp/oxygen.bmp");
    texture[16] = loadTextureFromFile("./bmp/sulfur.bmp");
    texture[17] = loadTextureFromFile("./bmp/chlorine.bmp");
    texture[35] = loadTextureFromFile("./bmp/bromine.bmp");

    setupLighting();
    setupFog();
    createMenu();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboardFunc);
    glutSpecialFunc(specialKeyboard);
    glutTimerFunc(16, timerCB, 0);

    glutMainLoop();
    return 0;
}
