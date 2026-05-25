# Shaders: Design & Architecture
**By Tadi — Day 0 brain-dump for the team**

> This document unfreezes the lighting pipeline. Read once, then code to it. If you need to change the approach, tell PM at the daily checkpoint.

---

## Overview

The TopFloor course uses **Blinn-Phong** forward rendering with multiple light sources:
- **Directional light** (sun by day, moon by night)
- **Point lights** (path lights, building lights — active only at night)
- **Spotlight** (drone lamp)
- **Ambient** (sky contribution, day-bright or night-cool)

All lighting is computed **in the fragment shader** per pixel. The **vertex shader** only transforms geometry and normals.

---

## Blinn-Phong Model (Per Fragment)

For each light source, we compute:

**Key difference from Phong:** We use the **half-vector** (normalize(lightDir + viewDir)) instead of reflection vector. It's cheaper and looks better.

---

## The Unified LightSet Struct

All lighting data goes into one `LightSet` struct (see `lighting/LightSet.h`):

```cpp
struct LightSet {
    DirectionalLight directional;  
    vec3 ambient;                  
    PointLight pointLights[4];     
    int numPointLights;
    SpotLight spotlight;           
    bool spotlightEnabled;
    bool isNight;                  
};
```

This struct is passed to **Scene::drawAllObjects()**. Each object's `draw()` receives it and uses it to populate shader uniforms.

---

## Vertex Shader (`blinn_phong.vert`)

**What it does:**
1. Transform vertex position to world space.
2. Transform normal to world space (using `mat3(transpose(inverse(model)))` to handle non-uniform scaling).
3. Pass both to fragment shader.

**What it does NOT do:**
- No light calculations here. Vertex lighting is for mobile; we compute per-fragment.

---

## Fragment Shader (`blinn_phong.frag`)

**Uniforms (passed by main.cpp):**
```glsl
uniform DirectionalLight directional;
uniform vec3 ambient;
uniform PointLight pointLights[4];
uniform int numPointLights;
uniform SpotLight spotlight;
uniform bool spotlightEnabled;
uniform bool isNight;
uniform vec3 cameraPos;
uniform vec3 objectColor;       
uniform float shininess = 32.0;
```

**Algorithm:**
1. Normalize the fragment normal.
2. Compute view direction (cameraPos − fragPos).
3. **Start with ambient ** = ambient × objectColor.
4. **Add directional light** (always).
5. **Add point lights** (only if `isNight`).
6. **Add spotlight** (only if `spotlightEnabled`).
7. Return final color.

---

## Point Light Attenuation

Distance-based falloff for point lights:

```glsl
distance = length(lightPos - fragPos);
attenuation = 1.0 / (1.0 + linear*d + quadratic*d²);
```

**Why quadratic?** Matches real-world inverse-square law. Linear term prevents singularities at d=0.

**Coefficients** (you'll tune these):
- Default: `linear = 0.09, quadratic = 0.032` for ~15m radius.
- Adjust `radius` in the LightSet for each light to control perceived size.

---

## Spotlight (Inner/Outer Cone)

Cone-based falloff with smooth edge transition:

```glsl
theta = dot(lightDir, normalize(-spotDir));
epsilon = innerCutoff − outerCutoff;
intensity = smoothstep(outerCutoff, innerCutoff, theta);  
```

**Angles stored as cosines:**
- `innerCutoff = cos(radians(12.5°))` ≈ 0.976 (tight beam)
- `outerCutoff = cos(radians(17.5°))` ≈ 0.956 (soft edge)

This gives a smooth gradient from full intensity (inside inner cone) → 0 (outside outer cone).

---

## Day/Night Toggle (`N` key)

**DAY:**
- Directional light = sun (direction ≈ upper-front-left)
- Directional color = warm white `(1.0, 0.95, 0.8)`
- Ambient = bright sky `(0.4, 0.42, 0.45)`
- Point lights = **disabled** (`numPointLights = 0`)
- Spotlight = usually off

**NIGHT:**
- Directional light = moon (different angle, same position roughly)
- Directional color = cool blue `(0.6, 0.6, 0.8)` (moonlight is bluish)
- Ambient = dark blue `(0.05, 0.05, 0.15)` (just a hint)
- Point lights = **enabled** (3–4 warm path/building lights)
- Spotlight = usually on (drone lamp)

**Implementation:**
- Main calls `updateLightingForDayNight(lights, isNight)` when user presses `N`.
- This function **remorphs the light data** in place (no reload, no pause).
- Next frame, new lighting is in effect.

---

## Integration Checklist (Karabo/integrator)

Before any object tries to use this lighting:

- [ ] `LightSet.h` is frozen and in `lighting/` folder.
- [ ] `blinn_phong.vert` and `blinn_phong.frag` are in `shaders/` folder.
- [ ] `ShaderManager::get("blinn_phong")` returns a compiled program.
- [ ] `main.cpp` calls `ShaderManager::load("blinn_phong", "blinn_phong.vert", "blinn_phong.frag")` at startup.
- [ ] Each object's `draw()` method receives `LightSet` and does **one `glUseProgram()` + pass uniforms**.

---

## Why This Approach?

| Choice | Reason |
|--------|--------|
| **Blinn-Phong, not PBR** | Faster, sufficient for stylized golf course, no material/roughness overhead. |
| **Per-fragment, not vertex** | Better quality shadows & specular highlights. Mobile would use vertex. |
| **Unified LightSet** | One struct, frozen interface, no surprises at integration. |
| **Cosine-stored angles for spotlight** | Avoids expensive `acos()` in shader; precalculated in CPU. |
| **Attenuation quadratic term** | Physically plausible; tunable via linear/quadratic coefficients. |
| **isNight flag, not time-based** | Instant toggle for screenshot demo; can add smooth transition later. |

---

## Tuning Knobs (Day 1–2, as you see objects)

Once you see the course lit up, tweak these:

```cpp



lights.directional.direction   
lights.directional.color       
lights.ambient                 


lights.directional.color       
lights.ambient                 
lights.pointLights[i].intensity 
lights.pointLights[i].position 
lights.pointLights[i].linear   
```

Tweak until it looks good. No magic numbers; just iterate.

---

## Potential Issues & Fixes

| Issue | Fix |
|-------|-----|
| Shadows too dark at night | Increase ambient value |
| Point lights look flat | Increase specular term or lower attenuation threshold |
| Spotlight too sharp | Increase `outerCutoff − innerCutoff` epsilon |
| Objects look washed out | Check `objectColor` — it should not be white |
| Specular too bright | Lower `shininess` (smaller exponent = wider highlight) |

---

## Next Steps

1. **This document** — Tadi (Day 0, done).
2. **Shader compilation** — ShaderManager must load `blinn_phong.vert` + `.frag` (Karabo integrates).
3. **Object integration** — Each object's `draw()` uses the LightSet to populate uniforms (per-object owner).
4. **Testing** — One lit cube on Day 0, course visible and lit by Day 1 AM.

---

**Questions? Ask at the 15-min checkpoint. Frozen signatures = no surprises.**