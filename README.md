# PBFluidSim – Particle-Based Fluid Simulation in Unreal Engine

## 📖 Overview
PBFluidSim is a **particle-based fluid simulation system** built as a modular **Unreal Engine 5 plugin**.  
The goal of this project is to implement **Smoothed Particle Hydrodynamics (SPH)** fluid behaviors with a **uniform spatial grid acceleration structure** for optimized neighbor lookups, enabling real-time interactive fluid simulation inside Unreal.

This project is part of a learning journey in **physics simulation, optimization, and engine development**.

---

## 🎯 SMART Goal
- **Specific**: Implement a particle-based fluid simulation plugin in Unreal Engine 5, using SPH interactions (cohesion, pressure, viscosity) and a uniform spatial grid acceleration structure.  
- **Measurable**: Success is defined by:
  1. Particles exhibit fluid-like behavior.
  2. Acceleration structure improves performance compared to naïve neighbor lookups.
  3. Simulation runs in real time with adjustable parameters (particle count, viscosity, container size).
  4. Plugin can be reused across other Unreal projects.  
- **Achievable**: By limiting scope to SPH + one acceleration structure, the project is challenging yet realistic for an **8-week timeframe**.  
- **Relevant**: Builds skills in **graphics programming, physics simulation, and Unreal plugin development**.  
- **Time-bound**: Completed in **8 weeks** with bi-weekly evaluations.

---

## 🗓️ Project Roadmap (Milestones)

**Week 1–2:**  
- Research SPH fluid simulation.  
- Implement a simple particle system under gravity & collisions.  
- Package into a modular plugin.  
✅ *Deliverable*: Prototype particle system with basic motion.  

**Week 3–4:**  
- Add fluid behaviors: cohesion, pressure, viscosity.  
- Validate stability at small particle counts.  
✅ *Deliverable*: Particles behave like a blob of fluid.  

**Week 5–6:**  
- Implement **uniform grid / spatial hashing** for neighbor lookups.  
- Benchmark vs. naïve implementation.  
✅ *Deliverable*: Demonstrated performance gains (more particles, smoother framerate).  

**Week 7–8:**  
- Polish simulation stability & parameters.  
- Add collisions with containers.  
- Build showcase demo scene.  
- Document math, implementation, & results.  
✅ *Deliverable*: Working fluid simulation plugin + demo.  

---

## 🚀 Getting Started

### Requirements
- **Unreal Engine 5.5+**  
- **Windows (Win64 build target)**  
- Visual Studio or VSCode (configured with Unreal includes)

### Setup
1. Clone the repository.  
2. Place the plugin in your Unreal project’s `Plugins/` directory.  
3. Open the `.uproject` file and regenerate project files.  
4. Build and run from Unreal Editor.

---

## 📂 Repository Structure
```
PBFluidSim/
├── Source/              # Plugin source code
├── Content/             # Demo content (materials, assets)
├── Config/              # Unreal configuration files
├── Binaries/, Saved/,   # Ignored build artifacts
├── Intermediate/        # Generated build files
└── PBFluidSim.uproject  # Unreal project file
```

---

## 📊 Progress Tracking
Progress is tracked against the milestones above. Bi-weekly updates include prototype demos, benchmarks, and stability reports.

---

## ✨ Acknowledgements
- Based on research in **Smoothed Particle Hydrodynamics (SPH)**.  
- Built with **Unreal Engine 5.5**.  
- Inspired by academic and industry work on real-time fluid simulation.  
