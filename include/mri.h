#ifndef MRI_H
#define MRI_H

#include "const.h"
#include "matrix.h"
#include "volume_io.h"
#include "box.h"

#define BUFTYPE  unsigned char

#define MRI_UCHAR   0
#define MRI_INT     1
#define MRI_LONG    2
#define MRI_FLOAT   3
#define MRI_SHORT   4

typedef struct
{
  int  x ;
  int  y ;
  int  z ;
  int  dx ;
  int  dy ;
  int  dz ;
} MRI_REGION ;

typedef struct
{
  int           width ;
  int           height ;
  int           depth ;     /* # of slices */
  int           type ;      /* data type for slices below */
  int           slice_direction ;   /* initially coronal, but may change */
  int           imnr0 ;     /* starting image # */
  int           imnr1 ;     /* ending image # */
  int           ptype ;     /* not used */
  float         fov ;
  float         thick ;
  float         ps ;   
  float         location ;  /* not used */
  float         xsize ;     /* size of a voxel in the x direction */ 
  float         ysize ;     /* size of a voxel in the y direction */ 
  float         zsize ;     /* size of a voxel in the z direction */ 
  float         xstart ;    /* start x (in xsize units) */
  float         xend ;      /* end x  (in xsize units) */
  float         ystart ;    /* start y   (in ysize units) */  
  float         yend ;      /* end y (in ysize units) */ 
  float         zstart ;    /* start z */  
  float         zend ;      /* end z */
  float         tr ;        /* time to recovery */
  float         te ;        /* time to echo */
  float         ti ;        /* time to inversion */
  char          fname[STR_LEN] ;

/* 
  each slice is an array of rows (mri->height of them) each of which is mri->width 
  long.
*/
  BUFTYPE       ***slices ;
  int           scale ;
  char          transform_fname[STR_LEN] ;
  General_transform transform ;   /* the next two are from this struct */
  Transform         *linear_transform ;
  Transform         *inverse_linear_transform ;
  int           free_transform ;   /* are we responsible for freeing it? */
  int           nframes ;          /* # of concatenated images */

  /* these are used to handle boundary conditions (arrays of indices) */
  int           *xi ;
  int           *yi ;
  int           *zi ;
} MRI_IMAGE, MRI ;

/* single pixel filtering */
float MRIvoxelMean(MRI *mri, int x, int y, int z, int wsize) ;
float MRIvoxelStd(MRI *mri, int x, int y, int z, float mean, int wsize) ;
float MRIvoxelZscore(MRI *mri, int x, int y, int z, int wsize) ;
float MRIvoxelDx(MRI *mri, int x, int y, int z) ;
float MRIvoxelDy(MRI *mri, int x, int y, int z) ;
float MRIvoxelDz(MRI *mri, int x, int y, int z) ;
float MRIvoxelGradient(MRI *mri, int x, int y, int z, float *pdx, float *pdy, 
                       float *pdz) ;
float MRIvoxelDirection(MRI *mri, int x, int y, int z, int wsize) ;

/* I/O functions */
int    MRIwrite(MRI *mri, char *fpref) ;
int    MRIwriteInfo(MRI *mri, char *fpref) ;
MRI   *MRIread(char *fpref) ;
MRI   *MRIreadInfo(char *fpref) ;

/* memory allocation routines */
int   MRIfree(MRI **pmri) ;
MRI   *MRIalloc(int width, int height, int depth, int type) ;
MRI   *MRIallocSequence(int width, int height,int depth,int type,int nframes);
MRI   *MRIallocHeader(int width, int height, int depth, int type) ;


/* correlation routines */
MRI   *MRIxcorr(MRI *mri_ref, MRI *mri_in, MRI *mri_dst) ;
MRI   *MRIxcorrWindow(MRI *mri_ref, MRI *mri_in,MRI *mri_dst,int window_size) ;
MRI   *MRInxcorr(MRI *mri_ref, MRI *mri_in, MRI *mri_dst) ;
MRI   *MRInxcorrWindow(MRI *mri_ref,MRI *mri_in,MRI *mri_dst,int window_size) ;
long  MRIcorrelate(MRI *mri_ref, MRI *mri_in, int xoff, int yoff, int zoff) ;


int   MRIpeak(MRI *mri, int *px, int *py, int *pz) ;
int   MRIcopyHeader(MRI *mri_src, MRI *mri_dst) ;
MRI   *MRIcopy(MRI *mri_src, MRI *mri_dst) ;
MRI   *MRIreslice(MRI *mri_src, MRI *mri_dst, int slice_direction) ;
int   MRIboundingBox(MRI *mri, int thresh, BOX *box) ;

/* coordinate transforms */
MRI   *MRItranslate(MRI *mri_src, MRI *mri_dst, int dx, int dy, int dz) ;
MRI   *MRIrotateX(MRI *mri_src, MRI *mri_dst, float x_angle) ;
MRI   *MRIrotateY(MRI *mri_src, MRI *mri_dst, float y_angle) ;
MRI   *MRIrotateZ(MRI *mri_src, MRI *mri_dst, float z_angle) ;
MRI   *MRIrotate(MRI *mri_src, MRI *mri_dst, MATRIX *mR, MATRIX *mO) ;
MRI   *MRIscale(MRI *mri_src, MRI *mri_dst, float sx, float sy, float sz) ;
MRI   *MRIaffine(MRI *mri_src, MRI *mri_dst, MATRIX *mA, MATRIX *mB) ;
MRI   *MRIconfThresh(MRI *mri_probs, MRI *mri_classes, MRI *mri_dst, 
                     float thresh, int target) ;

/* debugging */
int   MRIdump(MRI *mri, FILE *fp) ;
int   MRIdumpBuffer(MRI *mri, FILE *fp) ;

/* arithmetic operations */
MRI   *MRIsubtract(MRI *mri1, MRI *mri2, MRI *mri_dst) ;
MRI   *MRIabsdiff(MRI *mri1, MRI *mri2, MRI *mri_dst) ;
MRI   *MRIadd(MRI *mri1, MRI *mri2, MRI *mri_dst) ;
MRI   *MRIdivide(MRI *mri1, MRI *mri2, MRI *mri_dst) ;
MRI   *MRImultiply(MRI *mri1, MRI *mri2, MRI *mri_dst) ;
MRI   *MRIabs(MRI *mri, MRI *mri_dst) ;

/* filtering */
MRI   *MRIplaneOfLeastVarianceNormal(MRI *mri_src, MRI *mri_dst, int wsize) ;
MRI   *MRIpolvMean(MRI *mri_src, MRI *mri_dst, MRI *mri_polv, int wsize) ;

MRI   *MRIsobel(MRI *mri_src, MRI *mri_grad, MRI *mri_mag);
MRI   *MRIxSobel(MRI *mri_src, MRI *mri_x, int frame) ;
MRI   *MRIySobel(MRI *mri_src, MRI *mri_y, int frame) ;
MRI   *MRIzSobel(MRI *mri_src, MRI *mri_z, int frame) ;
MRI   *MRIreduce(MRI *mri_src, MRI *mri_dst) ;
MRI   *MRIconvolve1d(MRI *mri_src, MRI *mri_dst, float *kernel, 
                     int len, int axis) ;
MRI   *MRIreduce1d(MRI *mri_src, MRI *mri_dst,float *kernel,int len,int axis);
MRI   *MRIdiffuse(MRI *mri_src, MRI *mri_dst, double k, 
                    int niter, int which, double slope) ;
MRI   *MRIdiffuseCurvature(MRI *mri_src, MRI *mri_dst, 
                            double A,int niter, double slope) ;
MRI   *MRIdiffusePerona(MRI *mri_src, MRI *mri_dst, 
                             double k, int niter,double slope);
MRI   *MRIdirectionMap(MRI *mri_grad, MRI *mri_direction, int wsize);

/* offset stuff */
MRI   *MRIoffsetDirection(MRI *mri_grad, int wsize, MRI *mri_direction);
MRI   *MRIoffsetMagnitude(MRI *mri_src, MRI *mri_dst, int maxsteps) ;
MRI   *MRIapplyOffset(MRI *mri_src, MRI *mri_dst, MRI *mri_offset) ;


MRI   *MRIclone(MRI *mri_src, MRI *mri_dst) ;
MRI   *MRIthreshold(MRI *mri_src, MRI *mri_dst, BUFTYPE threshold) ;
int   MRIprincipleComponents(MRI *mri, MATRIX *mEvectors, float *evalues,
                              int *means, BUFTYPE theshold) ;
int   MRIclear(MRI *mri_src) ;

/* these routines use trilinear interpolation */
MRI   *MRIrotateX_I(MRI *mri_src, MRI *mri_dst, float x_angle) ;
MRI   *MRIrotateY_I(MRI *mri_src, MRI *mri_dst, float y_angle) ;
MRI   *MRIrotateZ_I(MRI *mri_src, MRI *mri_dst, float z_angle) ;
MRI   *MRIrotate_I(MRI *mri_src, MRI *mri_dst, MATRIX *mR, MATRIX *mO) ;

/* extraction routines */
MRI   *MRIextract(MRI *mri_src, MRI *mri_dst, int x0, int y0, int z0,
                  int dx, int dy, int dz) ;
MRI   *MRIextractInto(MRI *mri_src, MRI *mri_dst, int x0, int y0, int z0,
                  int dx, int dy, int dz, int x1, int y1, int z1) ;

MRI   *MRIextractPlanes(MRI *mri_src, MRI *mri_dst, MRI *mri_polv, int wsize,
                        int x, int y, int z);

/* morphology */
MRI   *MRImorph(MRI *mri_src, MRI *mri_dst, int which) ;
MRI   *MRIerode(MRI *mri_src, MRI *mri_dst) ;
MRI   *MRIdilate(MRI *mri_src, MRI *mri_dst) ;
MRI   *MRIopen(MRI *mri_src, MRI *mri_dst) ;
MRI   *MRIclose(MRI *mri_src, MRI *mri_dst) ;
/* the following use 4 (or 6 in 3-D) connectivity */
MRI   *MRIerode6(MRI *mri_src, MRI *mri_dst) ;
MRI   *MRIdilate6(MRI *mri_src, MRI *mri_dst) ;
MRI   *MRIopen6(MRI *mri_src, MRI *mri_dst) ;
MRI   *MRIclose6(MRI *mri_src, MRI *mri_dst) ;
MRI   *MRIunion(MRI *mri1, MRI *mri2, MRI *mri_dst) ;
MRI   *MRIintersect(MRI *mri1, MRI *mri2, MRI *mri_dst) ;
MRI   *MRIcomplement(MRI *mri_src, MRI *mri_dst) ;
MRI   *MRIxor(MRI *mri1, MRI *mri2, MRI *mri_dst, int t1, int t2) ;

/* filtering operations */
MRI   *MRIgaussian1d(float sigma, int max_len) ;
MRI   *MRIconvolveGaussian(MRI *mri_src, MRI *mri_dst, MRI *mri_gaussian) ;
MRI   *MRImean(MRI *mri_src, MRI *mri_dst, int wsize) ;
MRI   *MRImedian(MRI *mri_src, MRI *mri_dst, int wsize) ;
MRI   *MRIstd(MRI *mri_src, MRI*mri_dst, MRI *mri_mean, int wsize) ;
MRI   *MRInorm(MRI *mri_src, MRI*mri_dst, MRI *mri_mean, MRI *mri_std) ;

int   MRIcheckSize(MRI *mri_src, MRI *mri_check, int width, int height,
                   int depth) ;
MRI   *MRIreadRaw(FILE *fp, int width, int height, int depth, int type) ;
int   MRIinitHeader(MRI *mri) ;
int   MRIvoxelToWorld(MRI *mri, Real xv, Real yv, Real zv, 
                      Real *xw, Real *yw, Real *zw) ;
int   MRIworldToVoxel(MRI *mri, Real xw, Real yw, Real zw,
                Real *pxv, Real *pyv, Real *pzv) ;
int   MRIvoxelToTalairachVoxel(MRI *mri, Real xv, Real yv, Real zv,
                               Real *pxt, Real *pyt, Real *pzt) ;
int   MRItalairachVoxelToVoxel(MRI *mri, Real xt, Real yt, Real zt,
                               Real *pxv, Real *pyv, Real *pzv) ;
int   MRIvoxelToTalairach(MRI *mri, Real xv, Real yv, Real zv,
                               Real *pxt, Real *pyt, Real *pzt) ;
int   MRItalairachToVoxel(MRI *mri, Real xt, Real yt, Real zt,
                               Real *pxv, Real *pyv, Real *pzv) ;

int   MRItransformRegion(MRI *mri_src, MRI *mri_dst, MRI_REGION *src_region,
                                 MRI_REGION *dst_region) ;

#include "image.h"

IMAGE *MRItoImage(MRI *mri, IMAGE *I, int slice) ;
IMAGE *MRItoImageView(MRI *mri, IMAGE *I, int slice, int view, int frame) ;



#define MRISvox(mri,x,y,z)  (((short *)mri->slices[z][y])[x])
#define MRIFvox(mri,x,y,z)  (((float *)mri->slices[z][y])[x])
#define MRIvox(mri,x,y,z)   (((BUFTYPE *)mri->slices[z][y])[x])
#define MRIIvox(mri,x,y,z)  (((int *)mri->slices[z][y])[x])
#define MRILvox(mri,x,y,z)  (((long *)mri->slices[z][y])[x])

#define MRISseq_vox(mri,x,y,z,n)  (((short*)mri->slices[z+n*mri->depth][y])[x])
#define MRIFseq_vox(mri,x,y,z,n)  (((float*)mri->slices[z+n*mri->depth][y])[x])
#define MRIseq_vox(mri,x,y,z,n)   (((BUFTYPE *)\
                                    mri->slices[z+n*mri->depth][y])[x])
#define MRIIseq_vox(mri,x,y,z,n)  (((int *)mri->slices[z+n*mri->depth][y])[x])
#define MRILseq_vox(mri,x,y,z,n)  (((long *)mri->slices[z+n*mri->depth][y])[x])

#define MRI_HEIGHT      0
#define MRI_WIDTH       1
#define MRI_DEPTH       2

#define MRI_CORONAL     0
#define MRI_SAGITAL     1
#define MRI_HORIZONTAL  2
#define MRI_AXIAL       MRI_HORIZONTAL

/* vertices of an icasohedron (sp?), used by all POLV functions */
#define NVERTICES    21
extern float ic_x_vertices[]  ;
extern float ic_y_vertices[]  ;
extern float ic_z_vertices[]  ;


#include "histo.h"

int        MRIvalRange(MRI *mri, float *pmin, float *pmax) ;
HISTOGRAM  *MRIhistogram(MRI *mri, int nbins) ;
MRI        *MRIhistoEqualize(MRI *mri_src, MRI *mri_dst, int low) ;
MRI        *MRIapplyHistogram(MRI *mri_src, MRI *mri_dst, HISTOGRAM *histo) ;
MRI        *MRIcrunch(MRI *mri_src, MRI *mri_dst) ;
HISTOGRAM  *MRIgetEqualizeHisto(MRI *mri, HISTOGRAM *histo_eq, int low, 
                                int norm) ;
MRI        *MRIhistoNormalize(MRI *mri_src, MRI *mri_norm, MRI *mri_template,
                  int low) ;
MRI        *MRIadaptiveHistoNormalize(MRI *mri_src, MRI *mri_norm, 
                                      MRI *mri_template, int wsize,
                                      int hsize, int low) ;

/* these are adaptive (i.e. only operate on a subregion of the whole image */
MRI_REGION *MRIclipRegion(MRI *mri, MRI_REGION *reg_src, MRI_REGION *reg_clip);
int        MRIvalRangeRegion(MRI *mri, float *pmin, float *pmax, 
                             MRI_REGION *region) ;
HISTOGRAM  *MRIhistogramRegion(MRI *mri, int nbins, HISTOGRAM *histo,
                               MRI_REGION *region) ;
MRI        *MRIhistoEqualizeRegion(MRI *mri_src, MRI *mri_dst, int low, 
                                   MRI_REGION *region) ;
MRI        *MRIapplyHistogramToRegion(MRI *mri_src, MRI *mri_dst, 
                                    HISTOGRAM *histo, MRI_REGION *region) ;
HISTOGRAM  *MRIgetEqualizeHistoRegion(MRI *mri, HISTOGRAM *histo_eq, int low, 
                                      MRI_REGION *region, int norm) ;
MRI        *MRIhistoNormalizeRegion(MRI *mri_src, MRI *mri_norm, 
                                    MRI *mri_template, int low, 
                                    MRI_REGION *wreg, 
                                    MRI_REGION *h_src_reg, 
                                    MRI_REGION *h_tmp_reg) ;
MRI        *MRIextractRegion(MRI *mri_src, MRI *mri_dst, MRI_REGION *region) ;

#endif
