#ifndef FLUIDSIM_H
#define FLUIDSIM_H

#include <stdio.h>
#include <algorithm>

#define SWAP(x0, x) {float *tmp=x0; x0=x; x=tmp;}

class FluidSim {
private:

public:
    // w, h are dimensions of the entire grid.
    // Accounting for borders: simW = w-1, simH = h-1
    int w, h, simW, simH, size;
    float diffusion, viscosity, dt;

    // u: x-velocity  v: y-velocity   dens: density
    float *u, *u_prev, *v, *v_prev, *dens, *dens_prev;

    FluidSim(int w, int h, float diffusion, float viscosity, float dt)
        : w(w), h(h), simW(w-1), simH(h-1), diffusion(diffusion), viscosity(viscosity), dt(dt) {
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

    void addSource(float *grid, int x, int y, float dt) {
        grid[IX(x,y)] += dt;
    }

    // void addSource2(float *grid, int x, int y, float dt) {
    //     grid[IX(x, y)] = dt;
    // }

    void set_boundry(int w, int h, int b_flag, float *x) {
        // Left and right
        for (int i = 1; i < h; i++) {
            x[IX(0, i)]  = b_flag==1 ? -x[IX(1, i)] : x[IX(1, i)];
            x[IX(w-1,i)] = b_flag==1 ? -x[IX(w-2, i)] : x[IX(w-2, i)];
        }
        // Top and bottem
        for (int j = 1; j < w; j++) {
            x[IX(j, 0)]  = b_flag==2 ? -x[IX(j,1)] : x[IX(j,1)];
            x[IX(j,h-1)] = b_flag==2 ? -x[IX(j,h-2)] : x[IX(j,h-2)];
        }
        // Corners
        x[IX(0, 0)]     = 0.5*(x[IX(1,0 )]+x[IX(0 ,1)]);
        x[IX(0, h-1)]   = 0.5*(x[IX(1,h-1)]+x[IX(0 ,h-2)]);
        x[IX(w-1, 0)]   = 0.5*(x[IX(w-2,0 )]+x[IX(w-1,1)]);
        x[IX(w-1,h-1)]  = 0.5*(x[IX(w-2,h-1)]+x[IX(w-1,h-2 )]);
    }

    void diffuse(int w, int h, int b_flag, float *x, float *x0, float diff, float dt) {
        float a = dt * diff;

        int gauss_seidel_iterations = 20;
        for (int k = 0; k < gauss_seidel_iterations; k++) {
            for (int i = 1; i < w-1; i++) {
                for (int j = 1; j < h-1; j++) {
                    x[IX(i,j)] = (x0[IX(i,j)] + a *(x[IX(i-1,j)] +
                                                    x[IX(i+1,j)] +
                                                    x[IX(i,j-1)] +
                                                    x[IX(i,j+1)] )) /(1+4*a);
                }
            }
        }
        set_boundry(w, h, b_flag, x);
    }

    void advect(int w, int h, int b_flag, float *d, float *d0, float *u, float *v, float dt) {
        int i0, i1, j0, j1;
        float x, y, s0, s1, t0, t1;
        float dt0 = dt;

        // Linear interpolating the densities at their starting location from the 4 neighbors
        for (int i = 1; i < w-1; i++) {
            for (int j = 1 ; j < h-1; j++){
                // Calculate the source location
                x = i - dt0 * u[IX(i,j)];
                y = j - dt0 * v[IX(i,j)];
                
                if (x < 0.5)       { x=0.5; }
                if (x > (w-1+0.5)) { x=w-1+0.5; }
                i0 = (int)x, i1 = i0+1;
                if (y < 0.5)       { y=0.5; }
                if (y > (h-1+0.5)) { y=h-1+0.5; }
                j0 = (int)y, j1 = j0+1;

                s1 = x-i0, s0 = 1-s1;
                t1 = y-j0, t0 = 1-t1;
                // Sample from the 4 neighbors around the source point
                d[IX(i,j)] = s0 * (t0 * d0[IX(i0,j0)] + t1 * d0[IX(i0,j1)]) +
                             s1 * (t0 * d0[IX(i1,j0)] + t1 * d0[IX(i1,j1)]);
            }
        }
        set_boundry(w, h, b_flag, d);
    }

    void project(int w, int h, float *u, float *v, float *p, float *div) {
        float grid_spacing = 1.0/simH;

        for (int i = 1; i < w-1; i++) {
            for (int j = 1; j < h-1; j++) {
                div[IX(i,j)] = -0.5 * grid_spacing * (u[IX(i+1,j)] - u[IX(i-1, j)] +
                                                      v[IX(i,j+1)] - v[IX(i,j-1)]);
                p[IX(i,j)] = 0;
            }
        }
        set_boundry(w, h, 0, div);
        set_boundry(w, h, 0, p);

        int gauss_seidel_iterations = 20;
        for (int k = 0; k < gauss_seidel_iterations; k++) {
            for (int i = 1; i < w-1; i++) {
                for (int j = 1; j < h-1; j++) {
                    p[IX(i,j)] = (div[IX(i,j)] + p[IX(i-1,j)] + p[IX(i+1,j)] +
                                                 p[IX(i,j-1)] + p[IX(i,j+1)]) / 4;
                }
            }
            set_boundry(w, h, 0, p);
        }

        for (int i = 1; i < w-1; i++) {
            for (int j = 1; j < h-1; j++) {
                u[IX(i,j)] -= 0.5 * (p[IX(i+1,j)] - p[IX(i-1,j)]) / grid_spacing;
                v[IX(i,j)] -= 0.5 * (p[IX(i,j+1)] - p[IX(i,j-1)]) / grid_spacing;
            }
        }
        set_boundry(w, h, 1, u);
        set_boundry(w, h, 1, v);

    }

    void dens_step() {
        SWAP(dens, dens_prev);
        diffuse(w, h, 0, dens, dens_prev, diffusion, dt);
        SWAP(dens, dens_prev);
        advect(w, h, 0, dens, dens_prev, u, v, dt);
    }

    void vel_step() {
        SWAP(u, u_prev); diffuse(w, h, 1, u, u_prev, viscosity, dt);
        SWAP(v, v_prev); diffuse(w, h, 2, v, v_prev, viscosity, dt);
        project(w, h, u, v, u_prev, v_prev);
        SWAP(u, u_prev); SWAP(v, v_prev);
        advect(w, h, 1, u, u_prev, u_prev, v_prev, dt);
        advect(w, h, 2, v, v_prev, u_prev, v_prev, dt);
        project(w, h, u, v, u_prev, v_prev);
    }

    void update() {
        vel_step();
        dens_step();
    }
};

#endif // FluidSim