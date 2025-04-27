#ifndef FLUIDSIM_H
#define FLUIDSIM_H

class FluidSim {
private:

public:
  int w, h, size;
  float diffusion, viscosity, dt;

  // u: x-velocity  v: y-velocity   dens: density
  float *u, *u_prev, *v, *v_prev, *dens, *dens_prev;

  FluidSim(int w, int h, float diffusion, float viscosity, float dt)
    : w(w), h(h), diffusion(diffusion), viscosity(viscosity), dt(dt) {
    size = w * h;

    u         = new float[size] {};
    u_prev    = new float[size] {};
    v         = new float[size] {};
    v_prev    = new float[size] {};
    dens      = new float[size] {};
    dens_prev = new float[size] {};
  }

  ~FluidSim() {
    delete[] u;
    delete[] u_prev;
    delete[] v;
    delete[] v_prev;
    delete[] dens;
    delete[] dens_prev;
  }

  float* getDensity() const { return dens; }

  int IX(int i, int j) const {
    return i + w * j;
  }

  // Takes a set of coordinates (x, y) in the density map where sources should be added
  void addDensity(int n_points, int x, int y) {
    dens[IX(x, y)] += dt;
  }

  // void set_boundry();

  // void diffuse();
  // void advect();
  // void project();

  // void dens_step();
  // void vel_step();
};

#endif // FluidSim