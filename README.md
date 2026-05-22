# TopFloor-OpenGL — COS344 Miniature Golf Course

3D real-time render of the **Crescent Head Country Club Mini Golf Course** (18 holes) in **OpenGL 3.3 + C++11**.

> ⏱️ **We have ~2.5 days.** This README is built for a sprint: predefined interfaces, everyone works in isolation, integrate continuously. Pick tasks off the board — they are intentionally **not** assigned to names.

---

## Hard rules from the spec (read once, never break)

- **OpenGL 3.3** only. **C++11** only.
- **Allowed includes ONLY:** `stdio.h`, `stdlib.h`, `iostream`, `iomanip`, `cmath`, `sstream`, `GL/glew.h`, `GLFW/glfw3.h`, `glm/glm.hpp`, initialiser lists. Plus `shader.hpp` and `glad.c`. **⚠️ Confirm `stb_image.h` is allowed before relying on it** — it's not on the list. If it's out, we need a tiny custom image loader instead.
- **No pre-built objects or textures.** Every model and every texture must be made by us.
- **All helper files next to `main.cpp` in the submitted archive** (develop in `src/` subfolders, flatten for the final zip).
- **No inline implementation in headers** — bodies go in `.cpp`.
- **Blender** = individual objects only. C++ must combine them. No whole-course Blender render.
- **Doesn't compile → 0. Doesn't exit cleanly → 0.** Test both before submitting.
- **No AI-generated submitted code.** Plagiarism software runs on everything.
- Submit archive (Makefile + compile instructions) to **ClickUp**, upload to **FitchFork** at demo.

---

## How we work (so isolation actually works)

The whole point is that you can each build your slice **without waiting on anyone**. That only holds if the seams between slices are frozen first. So:

1. **Interface freeze comes before any feature code.** Day 0 we agree the shared headers (the contracts below). Once frozen, you code against the header — even if the other person's `.cpp` is empty, yours compiles and runs against a stub.
2. **Stubs everywhere.** Every shared class ships a do-nothing stub on Day 0 so `main` always compiles. You replace the stub body, never the signature, without a group ping.
3. **Branch per task, PR into `main`, one reviewer.** Small commits. Never push to `main` directly.
4. **Integration owner merges.** One person owns `main` and the Makefile so merges don't fight.
5. **Daily checkpoint (15 min, twice a day).** What compiles, what's blocked, what moved. PM runs it.

### The frozen contracts (agree these Day 0, then don't touch the signatures)

```cpp
// core/SceneObject.h — every visible thing implements this
class SceneObject {
public:
  virtual void build() = 0;                 // generate geometry into a VBO
  virtual void draw(const glm::mat4& view,
                    const glm::mat4& proj,
                    const LightSet& lights) = 0;
  virtual ~SceneObject() = default;
};

// physics/Collision.h — shared by obstacles, ball, drone
struct AABB           { glm::vec3 min, max; };
struct BoundingSphere { glm::vec3 c; float r; };
bool intersects(const AABB&, const AABB&);
bool intersects(const BoundingSphere&, const AABB&);

// lighting/LightSet.h — what the fragment shader reads
struct LightSet { /* dir light, point lights[], spotlight, ambient, time-of-day */ };

// core/ShaderManager.h — singleton, owns compiled programs
GLuint ShaderManager::get(const std::string& name);
```

> If you need to change a frozen signature, it goes through the PM and the daily checkpoint — never silently, because someone else is already coding against it.

---

## Repo structure

```
TopFloor-OpenGL/
├── src/
│   ├── main.cpp              # window, render loop, owns Scene
│   ├── core/                 # SceneObject base, Scene, ShaderManager, ResourceManager, Camera
│   ├── layout/               # Course, SkyBox, Terrain, Water
│   ├── objects/              # Hole, Windmill, Gazebo, Tree, Bridge, Rock, Ball, Drone
│   ├── lighting/             # LightSet, ShadowMap (FBO)
│   ├── physics/              # Collision (AABB/Sphere), BallPhysics
│   └── ui/                   # HUD, post-process colour filter
├── shaders/                  # blinn_phong, shadow_map, skybox, water, post_process
├── textures/                 # all custom textures
├── skybox/                   # day + night cube-map faces
├── docs/                     # report, annotated maps, reference images
├── Makefile
└── README.md
```

---

## Assignments

Each person owns a slice. Swaps are fine if two people agree — just tell the PM at checkpoint. The detailed task lists for each slice are in the board below.

| Person | Owns | Why this slice fits them | Marks in play |
|--------|------|--------------------------|----------------|
| **Njabulo** | **Slice D** — Holes, obstacles, windmill | Largest, most parallel chunk (18 holes + obstacles); rewards a fast, independent builder who can run solo | Objects 20 |
| **Karabo** | **Slice E** — Scene integration, physics, **integration owner** | Pure logic + plumbing, zero UI (his preference); owns the render pipeline, collision/ball physics, merges into `main`, and the Makefile | Code design 20 |
| **Bandile** | **Slice A** — Drone + camera | A complete self-contained subsystem he can take end-to-end without depending on anyone | Drone 8 |
| **Tadi** | **Slice B** — Lighting, shadows, skybox (**lead**) | His strongest area (shaders); applied where it matters most | Lighting 12 + Sky Box 2 |
| **Hayley (PM)** | **Slice 0** foundation + **Slice F** UI bonuses (HUD, colour filter) + **co-owns Slice B** + runs the sprint | Setup code everyone builds on + visible UI work she enjoys; stays close to lighting as the backup | Foundation + bonus + PM |

**Bonus ownership:** golf ball + ball physics → **Karabo** (it's physics); 3D grass → **Tadi** (shader work); HUD + colour filter → **Hayley**; water reflection → **Tadi/Hayley** as a stretch.

**Two safeguards baked in:**
- **Tadi co-owned, not solo.** Tadi leads lighting but Hayley co-owns Slice B, and Tadi's **first Day-0 task is a 30–60 min "shader brain-dump"** into `shaders/README.md` (Blinn-Phong + shadow-map approach). So lighting — the one slice that touches everything — can never stall on a single person. If Tadi delivers strongly, Hayley simply spends less time here; the hedge costs nothing.
- **Hayley is loaded across foundation + UI + pairing + PM.** Foundation is front-loaded to Day 0 (all-hands), and her solo HUD/colour-filter work is the most cuttable if she's stretched — drop those bonuses before anything core.

---

## The task board (your slice, branch, build against the frozen interface)

Tasks are grouped into **slices** built in isolation. Roughly ordered by how early they're needed. **Foundation is done first, together.**

### 🧱 Slice 0 — Foundation · **EVERYONE (Day 0)** → then split
*Lead: Hayley. A few hours, all-hands, before anyone goes solo.*
- [ ] GLFW window + OpenGL 3.3 context + render loop in `main.cpp`
- [ ] `Camera` (view/proj matrices) — even a fixed camera first
- [ ] `SceneObject` base + `Scene` container (`std::vector<SceneObject*>`, calls `draw()` on each)
- [ ] `ShaderManager` singleton + `blinn_phong` pair compiling → **one lit cube on screen**
- [ ] Primitive helpers: `makeCube/Cylinder/Cone/Sphere/Plane/CurvedStrip` → interleaved VBO data
- [ ] Tadi's **shader brain-dump** into `shaders/README.md`
- [ ] Stubs committed for every class below so `main` always compiles
- **Done when:** clean clone → `make` → window with one lit cube that the camera can see.

### 🚁 Slice A — Drone & camera · **BANDILE** — Drone 8 marks
- [ ] 6-DOF fly camera: translate right/up/forward + yaw/pitch/roll, **pitch clamped ±89°**
- [ ] Controls: WASD, Space/Ctrl, Q/E roll, mouse look, Shift boost (match the report's table)
- [ ] `glm::lookAt` rebuilt per frame; `glm::perspective` 45° FOV, far plane 200
- [ ] Drone body = scaled sphere shell + rotor arms; lit + **casts a shadow**
- [ ] Drone AABB collision vs terrain/solids (cancel motion on offending axis)

### 💡 Slice B — Lighting, shadows & skybox · **TADI (lead) + HAYLEY (co-owner)** — Lighting 12 + Sky Box 2 marks
- [ ] Blinn-Phong: directional sun/moon + point lights (attenuation) + drone spotlight (inner/outer cutoff) + ambient
- [ ] **Shadow mapping**: depth pass → FBO from sun's view; PCF + bias in main pass; every object casts a shadow
- [ ] Day/Night toggle (`N`): sun→moon, ambient→cool blue, point/path lights on
- [ ] Skybox: inward cube, cube-map sampler, depth-write off, translation-stripped view, **day AND night sets**

### 🌍 Slice C — Terrain, materials & water · **split: TADI (terrain+materials) / KARABO (water)** — Terrain 8 + Water 4 marks
*Terrain + materials are graphics-adjacent (Tadi). Water ties into render order + physics, so it sits with Karabo's Slice E. This is the one slice without a single owner — PM watch it closely, it's 12 marks.*
- [ ] **(Tadi)** Turf / concrete / sand / dirt meshes with **custom** textures, GL_REPEAT
- [ ] **(Tadi)** **Height-displaced sloped greens** (holes 6, 10, 12, 14, 18) — move vertex y, recompute normals
- [ ] **(Tadi)** Cascading waterfall (~5 m) as stacked primitives
- [ ] **(Karabo)** Water: flat mesh, alpha blend (`GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA`), high specular
- [ ] **(Tadi)** **Make all textures** — grass MUST visibly read as turf (reviewer flagged the old one)

> ⚠️ **PM note:** terrain/materials piles onto Tadi (already leading lighting). If he's overloaded or slipping, the textures task is the easiest to hand to Njabulo once his holes are built, and Hayley can absorb the sloped-greens geometry. Watch this at the Day-1 PM checkpoint.

### ⛳ Slice D — Holes, obstacles & windmill · **NJABULO** — Objects 20 marks
- [ ] All 18 holes from the report table (curved turf strips + caps, borders, flag + cup, start mat)
- [ ] Obstacles: rocks (5/9/11/16), tunnel (7), sand zones (4/11/17), narrow paths (3/18), raised (12/14)
- [ ] **Windmill on hole 8** (from a member's Practical 3): cylinder tower, cone roof, rotating blades
- [ ] Every obstacle gets an AABB or bounding sphere

### 🧩 Slice E — Scene integration & physics · **KARABO (also integration owner)** — Code design 20 marks
- [ ] `Scene` render order: shadow pass → skybox → opaque → transparent water → screen-space
- [ ] `physics/`: AABB/Sphere + ball movement, collision response (bounce/slow/stop), per-surface friction
- [ ] Decorations (≥5): gazebo, bridges, trees, benches, billboard, pathway lights — **Flyweight** for repeats
- [ ] Owns merging branches into `main` + the Makefile

### 🎮 Slice F — Bonus features (each = 5 marks) · owners tagged per line
- [ ] **(Karabo)** Practical-4 golf ball as the playable ball
- [ ] **(Karabo)** Ball movement + collision (overlaps Slice E physics)
- [ ] **(Tadi)** 3D grass (instanced blade quads on the turf)
- [ ] **(Tadi/Hayley, stretch)** Water reflection (planar: re-render mirrored scene → FBO → sample on water)
- [ ] **(Hayley)** Flight HUD (ortho screen-space: speed/altitude/orientation)
- [ ] **(Hayley)** Camera colour filter (full-screen post-process: grayscale/invert/mono)

---

## Bonus: claim or cut (only what's in the report counts)

All six are currently in the report. With ~2.5 days, **lock the high-value three, treat the rest as stretch.** If you cut one, delete its block from the report's Bonus section.

| Feature | Value/effort | Call |
|---------|--------------|------|
| Golf ball (Prac 4) | High / low | ✅ lock |
| Ball movement + collision | High / medium | ✅ lock |
| 3D grass | High / medium | ✅ if Slice C is ahead |
| Water reflection | Medium / medium-high | ⚠️ stretch |
| Flight HUD | Medium / low-medium | ⚠️ cut first if tight |
| Colour filter | Low / low | ⚠️ cut second if tight |

---

## 2.5-day sprint plan

| When | Goal |
|------|------|
| **Day 0 (now, evening)** | Freeze contracts. Foundation slice → one lit cube. Tadi's shader dump. All stubs committed. Claim slices. |
| **Day 1 AM** | Drone flies. First textured turf plane. 4–5 holes built. Basic day Blinn-Phong lighting. Scene draws everyone's stubs+real objects. |
| **Day 1 PM** | All 18 holes + obstacles + windmill. Shadow mapping working. Water + alpha. Day skybox. **Checkpoint: is the whole course visible and lit?** |
| **Day 2 AM** | Night mode + point/spot lights + night skybox. Ball physics + collision. Decorations placed. Lock the 3 core bonuses. |
| **Day 2 PM** | **Feature freeze.** Stretch bonuses only if green. Full integration test. Fix crashes. **Confirm clean exit.** |
| **Day 2.5 (final morning)** | Build archive, test Makefile on a CLEAN clone, finalise report (drop in all images, remove team notes), submit. |

> Long hours are fine, but **freeze features by Day 2 PM.** The last block is integration + the archive + Makefile test — that's where 0-mark traps hide.

---

## PM checklist (Hayley)

- [ ] Day 0: contracts frozen, stubs committed, slices claimed, Tadi's shader dump done.
- [ ] Twice-daily 15-min checkpoint: compiles? blocked? moved?
- [ ] Lighting/shadows is co-owned (Tadi lead + Hayley) — critical path can't stall on one person.
- [ ] Watch Slice C (terrain/materials/water, 12 marks, split owners) — reassign textures to Njabulo if Tadi overloads.
- [ ] No frozen signature changes without a checkpoint ping.
- [ ] Day 2 PM: call feature freeze out loud. Anything not working gets cut from code AND report.
- [ ] Final: clean-clone `make` test, clean-exit test, archive to ClickUp, FitchFork at demo.

---

## Submission checklist

- [ ] Report → **PDF**, all images in, team-note boxes removed.
- [ ] `make` works on a **clean clone**.
- [ ] Program **exits cleanly** (0-mark trap).
- [ ] Helper files **flattened next to `main.cpp`** in the archive.
- [ ] Makefile + compile instructions in the archive.
- [ ] No disallowed `#include`s anywhere.
- [ ] All textures + models are **ours**.
- [ ] Uploaded to **ClickUp** (report 22nd, program 25th); **FitchFork** at demo.
