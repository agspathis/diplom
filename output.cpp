#include "output.h"

void particles_to_vtk(const char* output_dir, fluid f, long frame)
{
    chdir(output_dir);
    char filename[sizeof "particles_000000.vtk"];
    sprintf(filename, "particles_%06d.vtk", frame);
    FILE* vtk = fopen(filename, "w");
    long size = f.particle_count;

    // standard header
    fprintf(vtk,"# vtk DataFile Version 3.1\n");
    fprintf(vtk,"Simulation particles\n");
    fprintf(vtk,"ASCII\n");
    fprintf(vtk,"DATASET UNSTRUCTURED_GRID\n");

    // point location
    fprintf(vtk, "POINTS %lu FLOAT\n", size);
    for (particle* pp=f.particles; pp<f.particles+size; pp++) {
	btVector3 position = particle_position(pp);
	fprintf(vtk, "%f %f %f\n", position.getX(), position.getY(), position.getZ());
    }
    fprintf(vtk, "\n");

    // dummy cell data
    fprintf(vtk, "CELLS 1 %lu\n", size+1);
    fprintf(vtk, "%lu", size);
    for (long i=0; i<size; i++) fprintf(vtk, " %lu", i);
    fprintf(vtk, "\n\n");

    fprintf(vtk, "CELL_TYPES 1\n2\n\n");

    // point data
    fprintf(vtk, "POINT_DATA %lu\n", size);
    fprintf(vtk, "VECTORS pressure DOUBLE\n");
    for (particle* pp=f.particles; pp<f.particles+size; pp++) {
	btVector3 pforce = pp->pforce;
	fprintf(vtk, "%f %f %f\n",
		pforce.getX() / f.particle_mass,
		pforce.getY() / f.particle_mass,
		pforce.getZ() / f.particle_mass);
    }
    fprintf(vtk, "\n");

    fprintf(vtk, "VECTORS viscosity DOUBLE\n");
    for (particle* pp=f.particles; pp<f.particles+size; pp++) {
	btVector3 vforce = pp->vforce;
	fprintf(vtk, "%f %f %f\n",
		vforce.getX() / f.particle_mass,
		vforce.getY() / f.particle_mass,
		vforce.getZ() / f.particle_mass);
    }
    fprintf(vtk, "\n");

    fprintf(vtk, "SCALARS density DOUBLE\n");
    fprintf(vtk, "LOOKUP_TABLE default\n");
    for (particle* pp=f.particles; pp<f.particles+size; pp++) {
	fprintf(vtk, "%f\n", pp->density/f.density);
    }
    fprintf(vtk, "\n");

    fclose(vtk);
}

void terrain_to_obj(const char* output_dir, terrain t)
{
    chdir(output_dir);
    FILE* vtk = fopen("terrain.obj", "w");
    fprintf(vtk, "o terrain\n");
    for (vertex* vp=t.vertices; vp<t.vertices + t.vertex_count; vp++)
	fprintf(vtk, "v %f %f %f\n", vp->x, vp->y, vp->z);
    for (face* fp=t.faces; fp<t.faces + t.face_count; fp++)
	fprintf(vtk, "f %d %d %d\n", fp->v0i+1, fp->v1i+1, fp->v2i+1);
    fclose(vtk);
}

void color_field_to_vtk(const char* output_dir, lp_grid lpg, long frame)
{
    chdir(output_dir);
    char filename[sizeof "color_field_000000.vtk"];
    sprintf(filename, "color_field_%06d.vtk", frame);
    FILE* vtk = fopen(filename, "w");
    long size = lpg.cell_count;

    // standard header
    fprintf(vtk, "# vtk DataFile Version 3.1\n");
    fprintf(vtk, "Simulation color field\n");
    fprintf(vtk, "ASCII\n");
    fprintf(vtk, "DATASET STRUCTURED_POINTS\n");
    fprintf(vtk, "DIMENSIONS %ld %ld %ld\n",
	    lpg.cf_sdf * lpg.x, lpg.cf_sdf * lpg.y, lpg.cf_sdf * lpg.z);
    fprintf(vtk, "ORIGIN %f %f %f\n",
	    lpg.origin.getX(), lpg.origin.getY(), lpg.origin.getZ());
    fprintf(vtk, "SPACING %f %f %f\n\n",
	    lpg.step / lpg.cf_sdf, lpg.step / lpg.cf_sdf, lpg.step / lpg.cf_sdf);
    
    fprintf(vtk, "POINT_DATA %ld\n", lpg.cell_count * (long) pow(lpg.cf_sdf, 3));
    fprintf(vtk, "SCALARS color_field FLOAT\n");
    fprintf(vtk, "LOOKUP_TABLE default\n");
    for (long cfi = 0; cfi < lpg.cell_count * pow(lpg.cf_sdf, 3); cfi++)
	fprintf(vtk, "%f\n", lpg.color_field[cfi]);
    fclose(vtk);
}
