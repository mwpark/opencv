/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2010-2012, Institute Of Software Chinese Academy Of Science, all rights reserved.
// Copyright (C) 2010-2012, Advanced Micro Devices, Inc., all rights reserved.
// Copyright (C) 2010-2012, Multicoreware, Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other oclMaterials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#ifndef __OPENCV_OCL_HPP__
#define __OPENCV_OCL_HPP__

#include <memory>
#include <vector>

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/features2d/features2d.hpp"

namespace cv
{
    namespace ocl
    {
        using std::auto_ptr;
        enum
        {
            CVCL_DEVICE_TYPE_DEFAULT     = (1 << 0),
            CVCL_DEVICE_TYPE_CPU         = (1 << 1),
            CVCL_DEVICE_TYPE_GPU         = (1 << 2),
            CVCL_DEVICE_TYPE_ACCELERATOR = (1 << 3),
            //CVCL_DEVICE_TYPE_CUSTOM      = (1 << 4)
            CVCL_DEVICE_TYPE_ALL         = 0xFFFFFFFF
        };
        //this class contains ocl runtime information
        class CV_EXPORTS Info
        {
        public:
            struct Impl;
            Impl *impl;

            Info();
            Info(const Info &m);
            ~Info();
            void release();
            Info &operator = (const Info &m);
            std::vector<string> DeviceName;
        };
        //////////////////////////////// Initialization & Info ////////////////////////
        //this function may be obsoleted
        //CV_EXPORTS cl_device_id getDevice();
        //the function must be called before any other cv::ocl::functions, it initialize ocl runtime
        //each Info relates to an OpenCL platform
        //there is one or more devices in each platform, each one has a separate name
        CV_EXPORTS int getDevice(std::vector<Info> &oclinfo, int devicetype = CVCL_DEVICE_TYPE_GPU);

        //set device you want to use, optional function after getDevice be called
        //the devnum is the index of the selected device in DeviceName vector of INfo
        CV_EXPORTS void setDevice(Info &oclinfo, int devnum = 0);

        //optional function, if you want save opencl binary kernel to the file, set its path
        CV_EXPORTS  void setBinpath(const char *path);

        //The two functions below enable other opencl program to use ocl module's cl_context and cl_command_queue
        CV_EXPORTS void* getoclContext();

        CV_EXPORTS void* getoclCommandQueue();

        //this function enable ocl module to use customized cl_context and cl_command_queue
        //getDevice also need to be called before this function
        CV_EXPORTS void setDeviceEx(Info &oclinfo, void *ctx, void *qu, int devnum = 0); 

        //////////////////////////////// Error handling ////////////////////////
        CV_EXPORTS void error(const char *error_string, const char *file, const int line, const char *func);

        //////////////////////////////// OpenCL context ////////////////////////
        //This is a global singleton class used to represent a OpenCL context.
        class Context
        {
        protected:
            Context();
            friend class auto_ptr<Context>;
            static auto_ptr<Context> clCxt;

        public:
            ~Context();
            static int val;
            static Context *getContext();
            static void setContext(Info &oclinfo);
            struct Impl;
            Impl *impl;
        };

        class CV_EXPORTS oclMatExpr;
        //////////////////////////////// oclMat ////////////////////////////////
        class CV_EXPORTS oclMat
        {
        public:
            //! default constructor
            oclMat();
            //! constructs oclMatrix of the specified size and type (_type is CV_8UC1, CV_64FC3, CV_32SC(12) etc.)
            oclMat(int rows, int cols, int type);
            oclMat(Size size, int type);
            //! constucts oclMatrix and fills it with the specified value _s.
            oclMat(int rows, int cols, int type, const Scalar &s);
            oclMat(Size size, int type, const Scalar &s);
            //! copy constructor
            oclMat(const oclMat &m);

            //! constructor for oclMatrix headers pointing to user-allocated data
            oclMat(int rows, int cols, int type, void *data, size_t step = Mat::AUTO_STEP);
            oclMat(Size size, int type, void *data, size_t step = Mat::AUTO_STEP);

            //! creates a matrix header for a part of the bigger matrix
            oclMat(const oclMat &m, const Range &rowRange, const Range &colRange);
            oclMat(const oclMat &m, const Rect &roi);

            //! builds oclMat from Mat. Perfom blocking upload to device.
            explicit oclMat (const Mat &m);

            //! destructor - calls release()
            ~oclMat();

            //! assignment operators
            oclMat &operator = (const oclMat &m);
            //! assignment operator. Perfom blocking upload to device.
            oclMat &operator = (const Mat &m);
            oclMat &operator = (const oclMatExpr& expr);

            //! pefroms blocking upload data to oclMat.
            void upload(const cv::Mat &m);


            //! downloads data from device to host memory. Blocking calls.
            operator Mat() const;
            void download(cv::Mat &m) const;


            //! returns a new oclMatrix header for the specified row
            oclMat row(int y) const;
            //! returns a new oclMatrix header for the specified column
            oclMat col(int x) const;
            //! ... for the specified row span
            oclMat rowRange(int startrow, int endrow) const;
            oclMat rowRange(const Range &r) const;
            //! ... for the specified column span
            oclMat colRange(int startcol, int endcol) const;
            oclMat colRange(const Range &r) const;

            //! returns deep copy of the oclMatrix, i.e. the data is copied
            oclMat clone() const;
            //! copies the oclMatrix content to "m".
            // It calls m.create(this->size(), this->type()).
            // It supports any data type
            void copyTo( oclMat &m ) const;
            //! copies those oclMatrix elements to "m" that are marked with non-zero mask elements.
            //It supports 8UC1 8UC4 32SC1 32SC4 32FC1 32FC4
            void copyTo( oclMat &m, const oclMat &mask ) const;
            //! converts oclMatrix to another datatype with optional scalng. See cvConvertScale.
            //It supports 8UC1 8UC4 32SC1 32SC4 32FC1 32FC4
            void convertTo( oclMat &m, int rtype, double alpha = 1, double beta = 0 ) const;

            void assignTo( oclMat &m, int type = -1 ) const;

            //! sets every oclMatrix element to s
            //It supports 8UC1 8UC4 32SC1 32SC4 32FC1 32FC4
            oclMat& operator = (const Scalar &s);
            //! sets some of the oclMatrix elements to s, according to the mask
            //It supports 8UC1 8UC4 32SC1 32SC4 32FC1 32FC4
            oclMat& setTo(const Scalar &s, const oclMat &mask = oclMat());
            //! creates alternative oclMatrix header for the same data, with different
            // number of channels and/or different number of rows. see cvReshape.
            oclMat reshape(int cn, int rows = 0) const;

            //! allocates new oclMatrix data unless the oclMatrix already has specified size and type.
            // previous data is unreferenced if needed.
            void create(int rows, int cols, int type);
            void create(Size size, int type);
            //! decreases reference counter;
            // deallocate the data when reference counter reaches 0.
            void release();

            //! swaps with other smart pointer
            void swap(oclMat &mat);

            //! locates oclMatrix header within a parent oclMatrix. See below
            void locateROI( Size &wholeSize, Point &ofs ) const;
            //! moves/resizes the current oclMatrix ROI inside the parent oclMatrix.
            oclMat& adjustROI( int dtop, int dbottom, int dleft, int dright );
            //! extracts a rectangular sub-oclMatrix
            // (this is a generalized form of row, rowRange etc.)
            oclMat operator()( Range rowRange, Range colRange ) const;
            oclMat operator()( const Rect &roi ) const;

            oclMat& operator+=( const oclMat& m );
            oclMat& operator-=( const oclMat& m );
            oclMat& operator*=( const oclMat& m );
            oclMat& operator/=( const oclMat& m );

            //! returns true if the oclMatrix data is continuous
            // (i.e. when there are no gaps between successive rows).
            // similar to CV_IS_oclMat_CONT(cvoclMat->type)
            bool isContinuous() const;
            //! returns element size in bytes,
            // similar to CV_ELEM_SIZE(cvMat->type)
            size_t elemSize() const;
            //! returns the size of element channel in bytes.
            size_t elemSize1() const;
            //! returns element type, similar to CV_MAT_TYPE(cvMat->type)
            int type() const;
            //! returns element type, i.e. 8UC3 returns 8UC4 because in ocl
            //! 3 channels element actually use 4 channel space
            int ocltype() const;
            //! returns element type, similar to CV_MAT_DEPTH(cvMat->type)
            int depth() const;
            //! returns element type, similar to CV_MAT_CN(cvMat->type)
            int channels() const;
            //! returns element type, return 4 for 3 channels element,
            //!becuase 3 channels element actually use 4 channel space
            int oclchannels() const;
            //! returns step/elemSize1()
            size_t step1() const;
            //! returns oclMatrix size:
            // width == number of columns, height == number of rows
            Size size() const;
            //! returns true if oclMatrix data is NULL
            bool empty() const;

            //! returns pointer to y-th row
            uchar* ptr(int y = 0);
            const uchar *ptr(int y = 0) const;

            //! template version of the above method
            template<typename _Tp> _Tp *ptr(int y = 0);
            template<typename _Tp> const _Tp *ptr(int y = 0) const;

            //! matrix transposition
            oclMat t() const;

            /*! includes several bit-fields:
              - the magic signature
              - continuity flag
              - depth
              - number of channels
              */
            int flags;
            //! the number of rows and columns
            int rows, cols;
            //! a distance between successive rows in bytes; includes the gap if any
            size_t step;
            //! pointer to the data(OCL memory object)
            uchar *data;

            //! pointer to the reference counter;
            // when oclMatrix points to user-allocated data, the pointer is NULL
            int *refcount;

            //! helper fields used in locateROI and adjustROI
            //datastart and dataend are not used in current version
            uchar *datastart;
            uchar *dataend;

            //! OpenCL context associated with the oclMat object.
            Context *clCxt;
            //add offset for handle ROI, calculated in byte
            int offset;
            //add wholerows and wholecols for the whole matrix, datastart and dataend are no longer used
            int wholerows;
            int wholecols;
        };


        ///////////////////// mat split and merge /////////////////////////////////
        //! Compose a multi-channel array from several single-channel arrays
        // Support all types
        CV_EXPORTS void merge(const oclMat *src, size_t n, oclMat &dst);
        CV_EXPORTS void merge(const vector<oclMat> &src, oclMat &dst);

        //! Divides multi-channel array into several single-channel arrays
        // Support all types
        CV_EXPORTS void split(const oclMat &src, oclMat *dst);
        CV_EXPORTS void split(const oclMat &src, vector<oclMat> &dst);

        ////////////////////////////// Arithmetics ///////////////////////////////////
        //#if defined DOUBLE_SUPPORT
        //typedef double F;
        //#else
        //typedef float F;
        //#endif
        //	CV_EXPORTS void addWeighted(const oclMat& a,F  alpha, const oclMat& b,F beta,F gama, oclMat& c);
        CV_EXPORTS void addWeighted(const oclMat &a, double  alpha, const oclMat &b, double beta, double gama, oclMat &c);
        //! adds one matrix to another (c = a + b)
        // supports all types except CV_8SC1,CV_8SC2,CV8SC3 and CV_8SC4
        CV_EXPORTS void add(const oclMat &a, const oclMat &b, oclMat &c);
        //! adds one matrix to another (c = a + b)
        // supports all types except CV_8SC1,CV_8SC2,CV8SC3 and CV_8SC4
        CV_EXPORTS void add(const oclMat &a, const oclMat &b, oclMat &c, const oclMat &mask);
        //! adds scalar to a matrix (c = a + s)
        // supports all types except CV_8SC1,CV_8SC2,CV8SC3 and CV_8SC4
        CV_EXPORTS void add(const oclMat &a, const Scalar &sc, oclMat &c, const oclMat &mask = oclMat());
        //! subtracts one matrix from another (c = a - b)
        // supports all types except CV_8SC1,CV_8SC2,CV8SC3 and CV_8SC4
        CV_EXPORTS void subtract(const oclMat &a, const oclMat &b, oclMat &c);
        //! subtracts one matrix from another (c = a - b)
        // supports all types except CV_8SC1,CV_8SC2,CV8SC3 and CV_8SC4
        CV_EXPORTS void subtract(const oclMat &a, const oclMat &b, oclMat &c, const oclMat &mask);
        //! subtracts scalar from a matrix (c = a - s)
        // supports all types except CV_8SC1,CV_8SC2,CV8SC3 and CV_8SC4
        CV_EXPORTS void subtract(const oclMat &a, const Scalar &sc, oclMat &c, const oclMat &mask = oclMat());
        //! subtracts scalar from a matrix (c = a - s)
        // supports all types except CV_8SC1,CV_8SC2,CV8SC3 and CV_8SC4
        CV_EXPORTS void subtract(const Scalar &sc, const oclMat &a, oclMat &c, const oclMat &mask = oclMat());
        //! computes element-wise product of the two arrays (c = a * b)
        // supports all types except CV_8SC1,CV_8SC2,CV8SC3 and CV_8SC4
        CV_EXPORTS void multiply(const oclMat &a, const oclMat &b, oclMat &c, double scale = 1);
        //! computes element-wise quotient of the two arrays (c = a / b)
        // supports all types except CV_8SC1,CV_8SC2,CV8SC3 and CV_8SC4
        CV_EXPORTS void divide(const oclMat &a, const oclMat &b, oclMat &c, double scale = 1);
        //! computes element-wise quotient of the two arrays (c = a / b)
        // supports all types except CV_8SC1,CV_8SC2,CV8SC3 and CV_8SC4
        CV_EXPORTS void divide(double scale, const oclMat &b, oclMat &c);

        //! compares elements of two arrays (c = a <cmpop> b)
        // supports except CV_8SC1,CV_8SC2,CV8SC3,CV_8SC4 types
        CV_EXPORTS void compare(const oclMat &a, const oclMat &b, oclMat &c, int cmpop);

        //! transposes the matrix
        // supports  CV_8UC1, 8UC4, 8SC4, 16UC2, 16SC2, 32SC1 and 32FC1.(the same as cuda)
        CV_EXPORTS void transpose(const oclMat &src, oclMat &dst);

        //! computes element-wise absolute difference of two arrays (c = abs(a - b))
        // supports all types except CV_8SC1,CV_8SC2,CV8SC3 and CV_8SC4
        CV_EXPORTS void absdiff(const oclMat &a, const oclMat &b, oclMat &c);
        //! computes element-wise absolute difference of array and scalar (c = abs(a - s))
        // supports all types except CV_8SC1,CV_8SC2,CV8SC3 and CV_8SC4
        CV_EXPORTS void absdiff(const oclMat &a, const Scalar &s, oclMat &c);

        //! computes mean value and standard deviation of all or selected array elements
        // supports except CV_32F,CV_64F
        CV_EXPORTS void meanStdDev(const oclMat &mtx, Scalar &mean, Scalar &stddev);

        //! computes norm of array
        // supports NORM_INF, NORM_L1, NORM_L2
        // supports only CV_8UC1 type
        CV_EXPORTS double norm(const oclMat &src1, int normType = NORM_L2);

        //! computes norm of the difference between two arrays
        // supports NORM_INF, NORM_L1, NORM_L2
        // supports only CV_8UC1 type
        CV_EXPORTS double norm(const oclMat &src1, const oclMat &src2, int normType = NORM_L2);

        //! reverses the order of the rows, columns or both in a matrix
        // supports all types
        CV_EXPORTS void flip(const oclMat &a, oclMat &b, int flipCode);

        //! computes sum of array elements
        // disabled until fix crash
        // support all types
        CV_EXPORTS Scalar sum(const oclMat &m);
        CV_EXPORTS Scalar absSum(const oclMat &m);
        CV_EXPORTS Scalar sqrSum(const oclMat &m);

        //! finds global minimum and maximum array elements and returns their values
        // support all C1 types

        CV_EXPORTS void minMax(const oclMat &src, double *minVal, double *maxVal = 0, const oclMat &mask = oclMat());

        //! finds global minimum and maximum array elements and returns their values with locations
        // support all C1 types

        CV_EXPORTS void minMaxLoc(const oclMat &src, double *minVal, double *maxVal = 0, Point *minLoc = 0, Point *maxLoc = 0,
                                  const oclMat &mask = oclMat());

        //! counts non-zero array elements
        // support all types
        CV_EXPORTS int countNonZero(const oclMat &src);

        //! transforms 8-bit unsigned integers using lookup table: dst(i)=lut(src(i))
        // destination array will have the depth type as lut and the same channels number as source
        //It supports 8UC1 8UC4 only
        CV_EXPORTS void LUT(const oclMat &src, const oclMat &lut, oclMat &dst);

        //! only 8UC1 and 256 bins is supported now
        CV_EXPORTS void calcHist(const oclMat &mat_src, oclMat &mat_hist);
        //! only 8UC1 and 256 bins is supported now
        CV_EXPORTS void equalizeHist(const oclMat &mat_src, oclMat &mat_dst);
        //! bilateralFilter
        // supports 8UC1 8UC4
        CV_EXPORTS void bilateralFilter(const oclMat& src, oclMat& dst, int d, double sigmaColor, double sigmaSpave, int borderType=BORDER_DEFAULT);
        //! computes exponent of each matrix element (b = e**a)
        // supports only CV_32FC1 type
        CV_EXPORTS void exp(const oclMat &a, oclMat &b);

        //! computes natural logarithm of absolute value of each matrix element: b = log(abs(a))
        // supports only CV_32FC1 type
        CV_EXPORTS void log(const oclMat &a, oclMat &b);

        //! computes magnitude of each (x(i), y(i)) vector
        // supports only CV_32F CV_64F type
        CV_EXPORTS void magnitude(const oclMat &x, const oclMat &y, oclMat &magnitude);
        CV_EXPORTS void magnitudeSqr(const oclMat &x, const oclMat &y, oclMat &magnitude);

        CV_EXPORTS void magnitudeSqr(const oclMat &x, oclMat &magnitude);

        //! computes angle (angle(i)) of each (x(i), y(i)) vector
        // supports only CV_32F CV_64F type
        CV_EXPORTS void phase(const oclMat &x, const oclMat &y, oclMat &angle, bool angleInDegrees = false);

        //! the function raises every element of tne input array to p
        //! support only CV_32F CV_64F type
        CV_EXPORTS void pow(const oclMat &x, double p, oclMat &y);

        //! converts Cartesian coordinates to polar
        // supports only CV_32F CV_64F type
        CV_EXPORTS void cartToPolar(const oclMat &x, const oclMat &y, oclMat &magnitude, oclMat &angle, bool angleInDegrees = false);

        //! converts polar coordinates to Cartesian
        // supports only CV_32F CV_64F type
        CV_EXPORTS void polarToCart(const oclMat &magnitude, const oclMat &angle, oclMat &x, oclMat &y, bool angleInDegrees = false);

        //! perfroms per-elements bit-wise inversion
        // supports all types
        CV_EXPORTS void bitwise_not(const oclMat &src, oclMat &dst);
        //! calculates per-element bit-wise disjunction of two arrays
        // supports all types
        CV_EXPORTS void bitwise_or(const oclMat &src1, const oclMat &src2, oclMat &dst, const oclMat &mask = oclMat());
        CV_EXPORTS void bitwise_or(const oclMat &src1, const Scalar &s, oclMat &dst, const oclMat &mask = oclMat());
        //! calculates per-element bit-wise conjunction of two arrays
        // supports all types
        CV_EXPORTS void bitwise_and(const oclMat &src1, const oclMat &src2, oclMat &dst, const oclMat &mask = oclMat());
        CV_EXPORTS void bitwise_and(const oclMat &src1, const Scalar &s, oclMat &dst, const oclMat &mask = oclMat());
        //! calculates per-element bit-wise "exclusive or" operation
        // supports all types
        CV_EXPORTS void bitwise_xor(const oclMat &src1, const oclMat &src2, oclMat &dst, const oclMat &mask = oclMat());
        CV_EXPORTS void bitwise_xor(const oclMat &src1, const Scalar &s, oclMat &dst, const oclMat &mask = oclMat());

        //! Logical operators
        CV_EXPORTS oclMatExpr operator ~ (const oclMat &src);
        CV_EXPORTS oclMatExpr operator | (const oclMat &src1, const oclMat &src2);
        CV_EXPORTS oclMatExpr operator & (const oclMat &src1, const oclMat &src2);
        CV_EXPORTS oclMatExpr operator ^ (const oclMat &src1, const oclMat &src2);

        //! Mathematics operators
        CV_EXPORTS oclMatExpr operator + (const oclMat &src1, const oclMat &src2);
        CV_EXPORTS oclMatExpr operator - (const oclMat &src1, const oclMat &src2);
        CV_EXPORTS oclMatExpr operator * (const oclMat &src1, const oclMat &src2);
        CV_EXPORTS oclMatExpr operator / (const oclMat &src1, const oclMat &src2);
 
        //! computes convolution of two images
        //! support only CV_32FC1 type
        CV_EXPORTS void convolve(const oclMat &image, const oclMat &temp1, oclMat &result);
 
        CV_EXPORTS void cvtColor(const oclMat &src, oclMat &dst, int code , int dcn = 0);

        //////////////////////////////// Filter Engine ////////////////////////////////

        /*!
          The Base Class for 1D or Row-wise Filters

          This is the base class for linear or non-linear filters that process 1D data.
          In particular, such filters are used for the "horizontal" filtering parts in separable filters.
          */
        class CV_EXPORTS BaseRowFilter_GPU
        {
        public:
            BaseRowFilter_GPU(int ksize_, int anchor_, int bordertype_) : ksize(ksize_), anchor(anchor_), bordertype(bordertype_) {}
            virtual ~BaseRowFilter_GPU() {}
            virtual void operator()(const oclMat &src, oclMat &dst) = 0;
            int ksize, anchor, bordertype;
        };

        /*!
          The Base Class for Column-wise Filters

          This is the base class for linear or non-linear filters that process columns of 2D arrays.
          Such filters are used for the "vertical" filtering parts in separable filters.
          */
        class CV_EXPORTS BaseColumnFilter_GPU
        {
        public:
            BaseColumnFilter_GPU(int ksize_, int anchor_, int bordertype_) : ksize(ksize_), anchor(anchor_), bordertype(bordertype_) {}
            virtual ~BaseColumnFilter_GPU() {}
            virtual void operator()(const oclMat &src, oclMat &dst) = 0;
            int ksize, anchor, bordertype;
        };

        /*!
          The Base Class for Non-Separable 2D Filters.

          This is the base class for linear or non-linear 2D filters.
          */
        class CV_EXPORTS BaseFilter_GPU
        {
        public:
            BaseFilter_GPU(const Size &ksize_, const Point &anchor_, const int &borderType_)
                : ksize(ksize_), anchor(anchor_), borderType(borderType_) {}
            virtual ~BaseFilter_GPU() {}
            virtual void operator()(const oclMat &src, oclMat &dst) = 0;
            Size ksize;
            Point anchor;
            int borderType;
        };

        /*!
          The Base Class for Filter Engine.

          The class can be used to apply an arbitrary filtering operation to an image.
          It contains all the necessary intermediate buffers.
          */
        class CV_EXPORTS FilterEngine_GPU
        {
        public:
            virtual ~FilterEngine_GPU() {}

            virtual void apply(const oclMat &src, oclMat &dst, Rect roi = Rect(0, 0, -1, -1)) = 0;
        };

        //! returns the non-separable filter engine with the specified filter
        CV_EXPORTS Ptr<FilterEngine_GPU> createFilter2D_GPU(const Ptr<BaseFilter_GPU> filter2D);

        //! returns the primitive row filter with the specified kernel
        CV_EXPORTS Ptr<BaseRowFilter_GPU> getLinearRowFilter_GPU(int srcType, int bufType, const Mat &rowKernel,
                int anchor = -1, int bordertype = BORDER_DEFAULT);

        //! returns the primitive column filter with the specified kernel
        CV_EXPORTS Ptr<BaseColumnFilter_GPU> getLinearColumnFilter_GPU(int bufType, int dstType, const Mat &columnKernel,
                int anchor = -1, int bordertype = BORDER_DEFAULT, double delta = 0.0);

        //! returns the separable linear filter engine
        CV_EXPORTS Ptr<FilterEngine_GPU> createSeparableLinearFilter_GPU(int srcType, int dstType, const Mat &rowKernel,
                const Mat &columnKernel, const Point &anchor = Point(-1, -1), double delta = 0.0, int bordertype = BORDER_DEFAULT);

        //! returns the separable filter engine with the specified filters
        CV_EXPORTS Ptr<FilterEngine_GPU> createSeparableFilter_GPU(const Ptr<BaseRowFilter_GPU> &rowFilter,
                const Ptr<BaseColumnFilter_GPU> &columnFilter);

        //! returns the Gaussian filter engine
        CV_EXPORTS Ptr<FilterEngine_GPU> createGaussianFilter_GPU(int type, Size ksize, double sigma1, double sigma2 = 0, int bordertype = BORDER_DEFAULT);

        //! returns filter engine for the generalized Sobel operator
        CV_EXPORTS Ptr<FilterEngine_GPU> createDerivFilter_GPU( int srcType, int dstType, int dx, int dy, int ksize, int borderType = BORDER_DEFAULT );

        //! applies Laplacian operator to the image
        // supports only ksize = 1 and ksize = 3 8UC1 8UC4 32FC1 32FC4 data type
        CV_EXPORTS void Laplacian(const oclMat &src, oclMat &dst, int ddepth, int ksize = 1, double scale = 1);

        //! returns 2D box filter
        // supports CV_8UC1 and CV_8UC4 source type, dst type must be the same as source type
        CV_EXPORTS Ptr<BaseFilter_GPU> getBoxFilter_GPU(int srcType, int dstType,
                const Size &ksize, Point anchor = Point(-1, -1), int borderType = BORDER_DEFAULT);

        //! returns box filter engine
        CV_EXPORTS Ptr<FilterEngine_GPU> createBoxFilter_GPU(int srcType, int dstType, const Size &ksize,
                const Point &anchor = Point(-1, -1), int borderType = BORDER_DEFAULT);

        //! returns 2D filter with the specified kernel
        // supports CV_8UC1 and CV_8UC4 types
        CV_EXPORTS Ptr<BaseFilter_GPU> getLinearFilter_GPU(int srcType, int dstType, const Mat &kernel, const Size &ksize,
                Point anchor = Point(-1, -1), int borderType = BORDER_DEFAULT);

        //! returns the non-separable linear filter engine
        CV_EXPORTS Ptr<FilterEngine_GPU> createLinearFilter_GPU(int srcType, int dstType, const Mat &kernel,
                const Point &anchor = Point(-1, -1), int borderType = BORDER_DEFAULT);

        //! smooths the image using the normalized box filter
        // supports data type: CV_8UC1, CV_8UC4, CV_32FC1 and CV_32FC4
        // supports border type: BORDER_CONSTANT, BORDER_REPLICATE, BORDER_REFLECT,BORDER_REFLECT_101,BORDER_WRAP
        CV_EXPORTS void boxFilter(const oclMat &src, oclMat &dst, int ddepth, Size ksize,
                                  Point anchor = Point(-1, -1), int borderType = BORDER_DEFAULT);

        //! returns 2D morphological filter
        //! only MORPH_ERODE and MORPH_DILATE are supported
        // supports CV_8UC1, CV_8UC4, CV_32FC1 and CV_32FC4 types
        // kernel must have CV_8UC1 type, one rows and cols == ksize.width * ksize.height
        CV_EXPORTS Ptr<BaseFilter_GPU> getMorphologyFilter_GPU(int op, int type, const Mat &kernel, const Size &ksize,
                Point anchor = Point(-1, -1));

        //! returns morphological filter engine. Only MORPH_ERODE and MORPH_DILATE are supported.
        CV_EXPORTS Ptr<FilterEngine_GPU> createMorphologyFilter_GPU(int op, int type, const Mat &kernel,
                const Point &anchor = Point(-1, -1), int iterations = 1);

        //! a synonym for normalized box filter
        // supports data type: CV_8UC1, CV_8UC4, CV_32FC1 and CV_32FC4
        // supports border type: BORDER_CONSTANT, BORDER_REPLICATE, BORDER_REFLECT,BORDER_REFLECT_101
        static inline void blur(const oclMat &src, oclMat &dst, Size ksize, Point anchor = Point(-1, -1),
                                int borderType = BORDER_CONSTANT)
        {
            boxFilter(src, dst, -1, ksize, anchor, borderType);
        }

        //! applies non-separable 2D linear filter to the image
        CV_EXPORTS void filter2D(const oclMat &src, oclMat &dst, int ddepth, const Mat &kernel,
                                 Point anchor = Point(-1, -1), int borderType = BORDER_DEFAULT);

        //! applies separable 2D linear filter to the image
        CV_EXPORTS void sepFilter2D(const oclMat &src, oclMat &dst, int ddepth, const Mat &kernelX, const Mat &kernelY,
                                    Point anchor = Point(-1, -1), double delta = 0.0, int bordertype = BORDER_DEFAULT);

        //! applies generalized Sobel operator to the image
        // dst.type must equalize src.type
        // supports data type: CV_8UC1, CV_8UC4, CV_32FC1 and CV_32FC4
        // supports border type: BORDER_CONSTANT, BORDER_REPLICATE, BORDER_REFLECT,BORDER_REFLECT_101
        CV_EXPORTS void Sobel(const oclMat &src, oclMat &dst, int ddepth, int dx, int dy, int ksize = 3, double scale = 1, double delta = 0.0, int bordertype = BORDER_DEFAULT);

        //! applies the vertical or horizontal Scharr operator to the image
        // dst.type must equalize src.type
        // supports data type: CV_8UC1, CV_8UC4, CV_32FC1 and CV_32FC4
        // supports border type: BORDER_CONSTANT, BORDER_REPLICATE, BORDER_REFLECT,BORDER_REFLECT_101
        CV_EXPORTS void Scharr(const oclMat &src, oclMat &dst, int ddepth, int dx, int dy, double scale = 1, double delta = 0.0, int bordertype = BORDER_DEFAULT);

        //! smooths the image using Gaussian filter.
        // dst.type must equalize src.type
        // supports data type: CV_8UC1, CV_8UC4, CV_32FC1 and CV_32FC4
        // supports border type: BORDER_CONSTANT, BORDER_REPLICATE, BORDER_REFLECT,BORDER_REFLECT_101
        CV_EXPORTS void GaussianBlur(const oclMat &src, oclMat &dst, Size ksize, double sigma1, double sigma2 = 0, int bordertype = BORDER_DEFAULT);

        //! erodes the image (applies the local minimum operator)
        // supports data type: CV_8UC1, CV_8UC4, CV_32FC1 and CV_32FC4
        CV_EXPORTS void erode( const oclMat &src, oclMat &dst, const Mat &kernel, Point anchor = Point(-1, -1), int iterations = 1,

                               int borderType = BORDER_CONSTANT, const Scalar &borderValue = morphologyDefaultBorderValue());


        //! dilates the image (applies the local maximum operator)
        // supports data type: CV_8UC1, CV_8UC4, CV_32FC1 and CV_32FC4
        CV_EXPORTS void dilate( const oclMat &src, oclMat &dst, const Mat &kernel, Point anchor = Point(-1, -1), int iterations = 1,

                                int borderType = BORDER_CONSTANT, const Scalar &borderValue = morphologyDefaultBorderValue());


        //! applies an advanced morphological operation to the image
        CV_EXPORTS void morphologyEx( const oclMat &src, oclMat &dst, int op, const Mat &kernel, Point anchor = Point(-1, -1), int iterations = 1,

                                      int borderType = BORDER_CONSTANT, const Scalar &borderValue = morphologyDefaultBorderValue());


        ////////////////////////////// Image processing //////////////////////////////
        //! Does mean shift filtering on GPU.
        CV_EXPORTS void meanShiftFiltering(const oclMat &src, oclMat &dst, int sp, int sr,
                                           TermCriteria criteria = TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 5, 1));

        //! Does mean shift procedure on GPU.
        CV_EXPORTS void meanShiftProc(const oclMat &src, oclMat &dstr, oclMat &dstsp, int sp, int sr,
                                      TermCriteria criteria = TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 5, 1));

        //! Does mean shift segmentation with elimiation of small regions.
        CV_EXPORTS void meanShiftSegmentation(const oclMat &src, Mat &dst, int sp, int sr, int minsize,
                                              TermCriteria criteria = TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 5, 1));

        //! applies fixed threshold to the image.
        // supports CV_8UC1 and CV_32FC1 data type
        // supports threshold type: THRESH_BINARY, THRESH_BINARY_INV, THRESH_TRUNC, THRESH_TOZERO, THRESH_TOZERO_INV
        CV_EXPORTS double threshold(const oclMat &src, oclMat &dst, double thresh, double maxVal, int type = THRESH_TRUNC);

        //! resizes the image
        // Supports INTER_NEAREST, INTER_LINEAR
        // supports CV_8UC1, CV_8UC4, CV_32FC1 and CV_32FC4 types
        CV_EXPORTS void resize(const oclMat &src, oclMat &dst, Size dsize, double fx = 0, double fy = 0, int interpolation = INTER_LINEAR);

        //! Applies a generic geometrical transformation to an image.

        // Supports INTER_NEAREST, INTER_LINEAR.

        // Map1 supports CV_16SC2, CV_32FC2  types.

        // Src supports CV_8UC1, CV_8UC2, CV_8UC4.

        CV_EXPORTS void remap(const oclMat &src, oclMat &dst, oclMat &map1, oclMat &map2, int interpolation, int bordertype, const Scalar &value = Scalar());

        //! copies 2D array to a larger destination array and pads borders with user-specifiable constant
        // supports CV_8UC1, CV_8UC4, CV_32SC1 types
        CV_EXPORTS void copyMakeBorder(const oclMat &src, oclMat &dst, int top, int bottom, int left, int right, int boardtype, const Scalar &value = Scalar());

        //! Smoothes image using median filter
        // The source 1- or 4-channel image. When m is 3 or 5, the image depth should be CV 8U or CV 32F.
        CV_EXPORTS void medianFilter(const oclMat &src, oclMat &dst, int m);

        //! warps the image using affine transformation
        // Supports INTER_NEAREST, INTER_LINEAR, INTER_CUBIC
        // supports CV_8UC1, CV_8UC4, CV_32FC1 and CV_32FC4 types
        CV_EXPORTS void warpAffine(const oclMat &src, oclMat &dst, const Mat &M, Size dsize, int flags = INTER_LINEAR);

        //! warps the image using perspective transformation
        // Supports INTER_NEAREST, INTER_LINEAR, INTER_CUBIC
        // supports CV_8UC1, CV_8UC4, CV_32FC1 and CV_32FC4 types
        CV_EXPORTS void warpPerspective(const oclMat &src, oclMat &dst, const Mat &M, Size dsize, int flags = INTER_LINEAR);

        //! computes the integral image and integral for the squared image
        // sum will have CV_32S type, sqsum - CV32F type
        // supports only CV_8UC1 source type
        CV_EXPORTS void integral(const oclMat &src, oclMat &sum, oclMat &sqsum);
        CV_EXPORTS void integral(const oclMat &src, oclMat &sum);
        CV_EXPORTS void cornerHarris(const oclMat &src, oclMat &dst, int blockSize, int ksize, double k, int bordertype = cv::BORDER_DEFAULT);
        CV_EXPORTS void cornerMinEigenVal(const oclMat &src, oclMat &dst, int blockSize, int ksize, int bordertype = cv::BORDER_DEFAULT);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////CascadeClassifier//////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        class CV_EXPORTS_W OclCascadeClassifier : public  cv::CascadeClassifier
        {
        public:
            OclCascadeClassifier() {};
            ~OclCascadeClassifier() {};

            CvSeq* oclHaarDetectObjects(oclMat &gimg, CvMemStorage *storage, double scaleFactor,
                                        int minNeighbors, int flags, CvSize minSize = cvSize(0, 0), CvSize maxSize = cvSize(0, 0));
        };



        /////////////////////////////// Pyramid /////////////////////////////////////
        CV_EXPORTS void pyrDown(const oclMat &src, oclMat &dst);

        //! upsamples the source image and then smoothes it
        CV_EXPORTS void pyrUp(const oclMat &src, oclMat &dst);

        //! performs linear blending of two images
        //! to avoid accuracy errors sum of weigths shouldn't be very close to zero
        // supports only CV_8UC1 source type
        CV_EXPORTS void blendLinear(const oclMat &img1, const oclMat &img2, const oclMat &weights1, const oclMat &weights2, oclMat &result);

        //! computes vertical sum, supports only CV_32FC1 images
        CV_EXPORTS void columnSum(const oclMat &src, oclMat &sum);

        ///////////////////////////////////////// match_template /////////////////////////////////////////////////////////////
        struct CV_EXPORTS MatchTemplateBuf
        {
            Size user_block_size;
            oclMat imagef, templf;
            std::vector<oclMat> images;
            std::vector<oclMat> image_sums;
            std::vector<oclMat> image_sqsums;
        };


        //! computes the proximity map for the raster template and the image where the template is searched for
        // Supports TM_SQDIFF, TM_SQDIFF_NORMED, TM_CCORR, TM_CCORR_NORMED, TM_CCOEFF, TM_CCOEFF_NORMED for type 8UC1 and 8UC4
        // Supports TM_SQDIFF, TM_CCORR for type 32FC1 and 32FC4
        CV_EXPORTS void matchTemplate(const oclMat &image, const oclMat &templ, oclMat &result, int method);

        //! computes the proximity map for the raster template and the image where the template is searched for
        // Supports TM_SQDIFF, TM_SQDIFF_NORMED, TM_CCORR, TM_CCORR_NORMED, TM_CCOEFF, TM_CCOEFF_NORMED for type 8UC1 and 8UC4
        // Supports TM_SQDIFF, TM_CCORR for type 32FC1 and 32FC4
        CV_EXPORTS void matchTemplate(const oclMat &image, const oclMat &templ, oclMat &result, int method, MatchTemplateBuf &buf);



        ///////////////////////////////////////////// Canny /////////////////////////////////////////////

        struct CV_EXPORTS CannyBuf;



        //! compute edges of the input image using Canny operator

        // Support CV_8UC1 only

        CV_EXPORTS void Canny(const oclMat &image, oclMat &edges, double low_thresh, double high_thresh, int apperture_size = 3, bool L2gradient = false);

        CV_EXPORTS void Canny(const oclMat &image, CannyBuf &buf, oclMat &edges, double low_thresh, double high_thresh, int apperture_size = 3, bool L2gradient = false);

        CV_EXPORTS void Canny(const oclMat &dx, const oclMat &dy, oclMat &edges, double low_thresh, double high_thresh, bool L2gradient = false);

        CV_EXPORTS void Canny(const oclMat &dx, const oclMat &dy, CannyBuf &buf, oclMat &edges, double low_thresh, double high_thresh, bool L2gradient = false);



        struct CV_EXPORTS CannyBuf

        {

            CannyBuf() : counter(NULL) {}

            ~CannyBuf()
            {
                release();
            }

            explicit CannyBuf(const Size &image_size, int apperture_size = 3) : counter(NULL)

            {

                create(image_size, apperture_size);

            }

            CannyBuf(const oclMat &dx_, const oclMat &dy_);



            void create(const Size &image_size, int apperture_size = 3);



            void release();



            oclMat dx, dy;

            oclMat dx_buf, dy_buf;

            oclMat edgeBuf;

            oclMat trackBuf1, trackBuf2;

            void *counter;

            Ptr<FilterEngine_GPU> filterDX, filterDY;

        };

        ///////////////////////////////////////// Hough Transform /////////////////////////////////////////
        //! HoughCircles
        struct HoughCirclesBuf
        {
            oclMat edges;
            oclMat accum;
            oclMat srcPoints;
            oclMat centers;
            CannyBuf cannyBuf;
        };

        CV_EXPORTS void HoughCircles(const oclMat& src, oclMat& circles, int method, float dp, float minDist, int cannyThreshold, int votesThreshold, int minRadius, int maxRadius, int maxCircles = 4096);
        CV_EXPORTS void HoughCircles(const oclMat& src, oclMat& circles, HoughCirclesBuf& buf, int method, float dp, float minDist, int cannyThreshold, int votesThreshold, int minRadius, int maxRadius, int maxCircles = 4096);
        CV_EXPORTS void HoughCirclesDownload(const oclMat& d_circles, OutputArray h_circles);

    
        ///////////////////////////////////////// clAmdFft related /////////////////////////////////////////
        //! Performs a forward or inverse discrete Fourier transform (1D or 2D) of floating point matrix.
        //! Param dft_size is the size of DFT transform.
        //!
        //! For complex-to-real transform it is assumed that the source matrix is packed in CLFFT's format.
        // support src type of CV32FC1, CV32FC2
        // support flags: DFT_INVERSE, DFT_REAL_OUTPUT, DFT_COMPLEX_OUTPUT, DFT_ROWS
        // dft_size is the size of original input, which is used for transformation from complex to real.
        // dft_size must be powers of 2, 3 and 5
        // real to complex dft requires at least v1.8 clAmdFft
        // real to complex dft output is not the same with cpu version
        // real to complex and complex to real does not support DFT_ROWS
        CV_EXPORTS void dft(const oclMat &src, oclMat &dst, Size dft_size = Size(0, 0), int flags = 0);

        //! implements generalized matrix product algorithm GEMM from BLAS
        // The functionality requires clAmdBlas library
        // only support type CV_32FC1
        // flag GEMM_3_T is not supported
        CV_EXPORTS void gemm(const oclMat &src1, const oclMat &src2, double alpha,
                             const oclMat &src3, double beta, oclMat &dst, int flags = 0);

        //////////////// HOG (Histogram-of-Oriented-Gradients) Descriptor and Object Detector //////////////

        struct CV_EXPORTS HOGDescriptor

        {

            enum { DEFAULT_WIN_SIGMA = -1 };

            enum { DEFAULT_NLEVELS = 64 };

            enum { DESCR_FORMAT_ROW_BY_ROW, DESCR_FORMAT_COL_BY_COL };



            HOGDescriptor(Size win_size = Size(64, 128), Size block_size = Size(16, 16),

                          Size block_stride = Size(8, 8), Size cell_size = Size(8, 8),

                          int nbins = 9, double win_sigma = DEFAULT_WIN_SIGMA,

                          double threshold_L2hys = 0.2, bool gamma_correction = true,

                          int nlevels = DEFAULT_NLEVELS);



            size_t getDescriptorSize() const;

            size_t getBlockHistogramSize() const;



            void setSVMDetector(const vector<float> &detector);



            static vector<float> getDefaultPeopleDetector();

            static vector<float> getPeopleDetector48x96();

            static vector<float> getPeopleDetector64x128();



            void detect(const oclMat &img, vector<Point> &found_locations,

                        double hit_threshold = 0, Size win_stride = Size(),

                        Size padding = Size());



            void detectMultiScale(const oclMat &img, vector<Rect> &found_locations,

                                  double hit_threshold = 0, Size win_stride = Size(),

                                  Size padding = Size(), double scale0 = 1.05,

                                  int group_threshold = 2);



            void getDescriptors(const oclMat &img, Size win_stride,

                                oclMat &descriptors,

                                int descr_format = DESCR_FORMAT_COL_BY_COL);



            Size win_size;

            Size block_size;

            Size block_stride;

            Size cell_size;

            int nbins;

            double win_sigma;

            double threshold_L2hys;

            bool gamma_correction;

            int nlevels;



        protected:

            // initialize buffers; only need to do once in case of multiscale detection

            void init_buffer(const oclMat &img, Size win_stride);



            void computeBlockHistograms(const oclMat &img);

            void computeGradient(const oclMat &img, oclMat &grad, oclMat &qangle);



            double getWinSigma() const;

            bool checkDetectorSize() const;



            static int numPartsWithin(int size, int part_size, int stride);

            static Size numPartsWithin(Size size, Size part_size, Size stride);



            // Coefficients of the separating plane

            float free_coef;

            oclMat detector;



            // Results of the last classification step

            oclMat labels;

            Mat labels_host;



            // Results of the last histogram evaluation step

            oclMat block_hists;



            // Gradients conputation results

            oclMat grad, qangle;



            // scaled image

            oclMat image_scale;



            // effect size of input image (might be different from original size after scaling)

            Size effect_size;

        };



        //! Speeded up robust features, port from GPU module.
        ////////////////////////////////// SURF //////////////////////////////////////////

        class CV_EXPORTS SURF_OCL

        {

        public:

            enum KeypointLayout

            {

                X_ROW = 0,

                Y_ROW,

                LAPLACIAN_ROW,

                OCTAVE_ROW,

                SIZE_ROW,

                ANGLE_ROW,

                HESSIAN_ROW,

                ROWS_COUNT

            };



            //! the default constructor

            SURF_OCL();

            //! the full constructor taking all the necessary parameters

            explicit SURF_OCL(double _hessianThreshold, int _nOctaves = 4,

                              int _nOctaveLayers = 2, bool _extended = false, float _keypointsRatio = 0.01f, bool _upright = false);



            //! returns the descriptor size in float's (64 or 128)

            int descriptorSize() const;



            //! upload host keypoints to device memory

            void uploadKeypoints(const vector<cv::KeyPoint> &keypoints, oclMat &keypointsocl);

            //! download keypoints from device to host memory

            void downloadKeypoints(const oclMat &keypointsocl, vector<KeyPoint> &keypoints);



            //! download descriptors from device to host memory

            void downloadDescriptors(const oclMat &descriptorsocl, vector<float> &descriptors);



            //! finds the keypoints using fast hessian detector used in SURF

            //! supports CV_8UC1 images

            //! keypoints will have nFeature cols and 6 rows

            //! keypoints.ptr<float>(X_ROW)[i] will contain x coordinate of i'th feature

            //! keypoints.ptr<float>(Y_ROW)[i] will contain y coordinate of i'th feature

            //! keypoints.ptr<float>(LAPLACIAN_ROW)[i] will contain laplacian sign of i'th feature

            //! keypoints.ptr<float>(OCTAVE_ROW)[i] will contain octave of i'th feature

            //! keypoints.ptr<float>(SIZE_ROW)[i] will contain size of i'th feature

            //! keypoints.ptr<float>(ANGLE_ROW)[i] will contain orientation of i'th feature

            //! keypoints.ptr<float>(HESSIAN_ROW)[i] will contain response of i'th feature

            void operator()(const oclMat &img, const oclMat &mask, oclMat &keypoints);

            //! finds the keypoints and computes their descriptors.

            //! Optionally it can compute descriptors for the user-provided keypoints and recompute keypoints direction

            void operator()(const oclMat &img, const oclMat &mask, oclMat &keypoints, oclMat &descriptors,

                            bool useProvidedKeypoints = false);



            void operator()(const oclMat &img, const oclMat &mask, std::vector<KeyPoint> &keypoints);

            void operator()(const oclMat &img, const oclMat &mask, std::vector<KeyPoint> &keypoints, oclMat &descriptors,

                            bool useProvidedKeypoints = false);



            void operator()(const oclMat &img, const oclMat &mask, std::vector<KeyPoint> &keypoints, std::vector<float> &descriptors,

                            bool useProvidedKeypoints = false);



            void releaseMemory();



            // SURF parameters

            float hessianThreshold;

            int nOctaves;

            int nOctaveLayers;

            bool extended;

            bool upright;



            //! max keypoints = min(keypointsRatio * img.size().area(), 65535)

            float keypointsRatio;



            oclMat sum, mask1, maskSum, intBuffer;



            oclMat det, trace;



            oclMat maxPosBuffer;

        };

        ////////////////////////feature2d_ocl/////////////////
        /****************************************************************************************\
        *                                      Distance                                          *
        \****************************************************************************************/

        template<typename T>
        struct CV_EXPORTS Accumulator
        {
            typedef T Type;
        };

        template<> struct Accumulator<unsigned char>
        {
            typedef float Type;
        };
        template<> struct Accumulator<unsigned short>
        {
            typedef float Type;
        };
        template<> struct Accumulator<char>
        {
            typedef float Type;
        };
        template<> struct Accumulator<short>
        {
            typedef float Type;
        };

        /*
         * Manhattan distance (city block distance) functor
         */
        template<class T>
        struct CV_EXPORTS L1
        {
            enum { normType = NORM_L1 };
            typedef T ValueType;
            typedef typename Accumulator<T>::Type ResultType;

            ResultType operator()( const T *a, const T *b, int size ) const
            {
                return normL1<ValueType, ResultType>(a, b, size);
            }
        };

        /*
         * Euclidean distance functor
         */
        template<class T>
        struct CV_EXPORTS L2
        {
            enum { normType = NORM_L2 };
            typedef T ValueType;
            typedef typename Accumulator<T>::Type ResultType;

            ResultType operator()( const T *a, const T *b, int size ) const
            {
                return (ResultType)sqrt((double)normL2Sqr<ValueType, ResultType>(a, b, size));
            }
        };

        /*
         * Hamming distance functor - counts the bit differences between two strings - useful for the Brief descriptor
         * bit count of A exclusive XOR'ed with B
         */
        struct CV_EXPORTS Hamming
        {
            enum { normType = NORM_HAMMING };
            typedef unsigned char ValueType;
            typedef int ResultType;

            /** this will count the bits in a ^ b
             */
            ResultType operator()( const unsigned char *a, const unsigned char *b, int size ) const
            {
                return normHamming(a, b, size);
            }
        };

        ////////////////////////////////// BruteForceMatcher //////////////////////////////////

        class CV_EXPORTS BruteForceMatcher_OCL_base
        {
        public:
            enum DistType {L1Dist = 0, L2Dist, HammingDist};

            explicit BruteForceMatcher_OCL_base(DistType distType = L2Dist);



            // Add descriptors to train descriptor collection

            void add(const std::vector<oclMat> &descCollection);



            // Get train descriptors collection

            const std::vector<oclMat> &getTrainDescriptors() const;



            // Clear train descriptors collection

            void clear();



            // Return true if there are not train descriptors in collection

            bool empty() const;



            // Return true if the matcher supports mask in match methods

            bool isMaskSupported() const;



            // Find one best match for each query descriptor

            void matchSingle(const oclMat &query, const oclMat &train,

                             oclMat &trainIdx, oclMat &distance,

                             const oclMat &mask = oclMat());



            // Download trainIdx and distance and convert it to CPU vector with DMatch

            static void matchDownload(const oclMat &trainIdx, const oclMat &distance, std::vector<DMatch> &matches);

            // Convert trainIdx and distance to vector with DMatch

            static void matchConvert(const Mat &trainIdx, const Mat &distance, std::vector<DMatch> &matches);



            // Find one best match for each query descriptor

            void match(const oclMat &query, const oclMat &train, std::vector<DMatch> &matches, const oclMat &mask = oclMat());



            // Make gpu collection of trains and masks in suitable format for matchCollection function

            void makeGpuCollection(oclMat &trainCollection, oclMat &maskCollection, const std::vector<oclMat> &masks = std::vector<oclMat>());



            // Find one best match from train collection for each query descriptor

            void matchCollection(const oclMat &query, const oclMat &trainCollection,

                                 oclMat &trainIdx, oclMat &imgIdx, oclMat &distance,

                                 const oclMat &masks = oclMat());



            // Download trainIdx, imgIdx and distance and convert it to vector with DMatch

            static void matchDownload(const oclMat &trainIdx, const oclMat &imgIdx, const oclMat &distance, std::vector<DMatch> &matches);

            // Convert trainIdx, imgIdx and distance to vector with DMatch

            static void matchConvert(const Mat &trainIdx, const Mat &imgIdx, const Mat &distance, std::vector<DMatch> &matches);



            // Find one best match from train collection for each query descriptor.

            void match(const oclMat &query, std::vector<DMatch> &matches, const std::vector<oclMat> &masks = std::vector<oclMat>());



            // Find k best matches for each query descriptor (in increasing order of distances)

            void knnMatchSingle(const oclMat &query, const oclMat &train,

                                oclMat &trainIdx, oclMat &distance, oclMat &allDist, int k,

                                const oclMat &mask = oclMat());



            // Download trainIdx and distance and convert it to vector with DMatch

            // compactResult is used when mask is not empty. If compactResult is false matches

            // vector will have the same size as queryDescriptors rows. If compactResult is true

            // matches vector will not contain matches for fully masked out query descriptors.

            static void knnMatchDownload(const oclMat &trainIdx, const oclMat &distance,

                                         std::vector< std::vector<DMatch> > &matches, bool compactResult = false);

            // Convert trainIdx and distance to vector with DMatch

            static void knnMatchConvert(const Mat &trainIdx, const Mat &distance,

                                        std::vector< std::vector<DMatch> > &matches, bool compactResult = false);



            // Find k best matches for each query descriptor (in increasing order of distances).

            // compactResult is used when mask is not empty. If compactResult is false matches

            // vector will have the same size as queryDescriptors rows. If compactResult is true

            // matches vector will not contain matches for fully masked out query descriptors.

            void knnMatch(const oclMat &query, const oclMat &train,

                          std::vector< std::vector<DMatch> > &matches, int k, const oclMat &mask = oclMat(),

                          bool compactResult = false);



            // Find k best matches from train collection for each query descriptor (in increasing order of distances)

            void knnMatch2Collection(const oclMat &query, const oclMat &trainCollection,

                                     oclMat &trainIdx, oclMat &imgIdx, oclMat &distance,

                                     const oclMat &maskCollection = oclMat());



            // Download trainIdx and distance and convert it to vector with DMatch

            // compactResult is used when mask is not empty. If compactResult is false matches

            // vector will have the same size as queryDescriptors rows. If compactResult is true

            // matches vector will not contain matches for fully masked out query descriptors.

            static void knnMatch2Download(const oclMat &trainIdx, const oclMat &imgIdx, const oclMat &distance,

                                          std::vector< std::vector<DMatch> > &matches, bool compactResult = false);

            // Convert trainIdx and distance to vector with DMatch

            static void knnMatch2Convert(const Mat &trainIdx, const Mat &imgIdx, const Mat &distance,

                                         std::vector< std::vector<DMatch> > &matches, bool compactResult = false);



            // Find k best matches  for each query descriptor (in increasing order of distances).

            // compactResult is used when mask is not empty. If compactResult is false matches

            // vector will have the same size as queryDescriptors rows. If compactResult is true

            // matches vector will not contain matches for fully masked out query descriptors.

            void knnMatch(const oclMat &query, std::vector< std::vector<DMatch> > &matches, int k,

                          const std::vector<oclMat> &masks = std::vector<oclMat>(), bool compactResult = false);



            // Find best matches for each query descriptor which have distance less than maxDistance.

            // nMatches.at<int>(0, queryIdx) will contain matches count for queryIdx.

            // carefully nMatches can be greater than trainIdx.cols - it means that matcher didn't find all matches,

            // because it didn't have enough memory.

            // If trainIdx is empty, then trainIdx and distance will be created with size nQuery x max((nTrain / 100), 10),

            // otherwize user can pass own allocated trainIdx and distance with size nQuery x nMaxMatches

            // Matches doesn't sorted.

            void radiusMatchSingle(const oclMat &query, const oclMat &train,

                                   oclMat &trainIdx, oclMat &distance, oclMat &nMatches, float maxDistance,

                                   const oclMat &mask = oclMat());



            // Download trainIdx, nMatches and distance and convert it to vector with DMatch.

            // matches will be sorted in increasing order of distances.

            // compactResult is used when mask is not empty. If compactResult is false matches

            // vector will have the same size as queryDescriptors rows. If compactResult is true

            // matches vector will not contain matches for fully masked out query descriptors.

            static void radiusMatchDownload(const oclMat &trainIdx, const oclMat &distance, const oclMat &nMatches,

                                            std::vector< std::vector<DMatch> > &matches, bool compactResult = false);

            // Convert trainIdx, nMatches and distance to vector with DMatch.

            static void radiusMatchConvert(const Mat &trainIdx, const Mat &distance, const Mat &nMatches,

                                           std::vector< std::vector<DMatch> > &matches, bool compactResult = false);



            // Find best matches for each query descriptor which have distance less than maxDistance

            // in increasing order of distances).

            void radiusMatch(const oclMat &query, const oclMat &train,

                             std::vector< std::vector<DMatch> > &matches, float maxDistance,

                             const oclMat &mask = oclMat(), bool compactResult = false);



            // Find best matches for each query descriptor which have distance less than maxDistance.

            // If trainIdx is empty, then trainIdx and distance will be created with size nQuery x max((nQuery / 100), 10),

            // otherwize user can pass own allocated trainIdx and distance with size nQuery x nMaxMatches

            // Matches doesn't sorted.

            void radiusMatchCollection(const oclMat &query, oclMat &trainIdx, oclMat &imgIdx, oclMat &distance, oclMat &nMatches, float maxDistance,

                                       const std::vector<oclMat> &masks = std::vector<oclMat>());



            // Download trainIdx, imgIdx, nMatches and distance and convert it to vector with DMatch.

            // matches will be sorted in increasing order of distances.

            // compactResult is used when mask is not empty. If compactResult is false matches

            // vector will have the same size as queryDescriptors rows. If compactResult is true

            // matches vector will not contain matches for fully masked out query descriptors.

            static void radiusMatchDownload(const oclMat &trainIdx, const oclMat &imgIdx, const oclMat &distance, const oclMat &nMatches,

                                            std::vector< std::vector<DMatch> > &matches, bool compactResult = false);

            // Convert trainIdx, nMatches and distance to vector with DMatch.

            static void radiusMatchConvert(const Mat &trainIdx, const Mat &imgIdx, const Mat &distance, const Mat &nMatches,

                                           std::vector< std::vector<DMatch> > &matches, bool compactResult = false);



            // Find best matches from train collection for each query descriptor which have distance less than

            // maxDistance (in increasing order of distances).

            void radiusMatch(const oclMat &query, std::vector< std::vector<DMatch> > &matches, float maxDistance,

                             const std::vector<oclMat> &masks = std::vector<oclMat>(), bool compactResult = false);



            DistType distType;



        private:

            std::vector<oclMat> trainDescCollection;

        };



        template <class Distance>

        class CV_EXPORTS BruteForceMatcher_OCL;



        template <typename T>

        class CV_EXPORTS BruteForceMatcher_OCL< L1<T> > : public BruteForceMatcher_OCL_base

        {

        public:

            explicit BruteForceMatcher_OCL() : BruteForceMatcher_OCL_base(L1Dist) {}

            explicit BruteForceMatcher_OCL(L1<T> /*d*/) : BruteForceMatcher_OCL_base(L1Dist) {}

        };

        template <typename T>

        class CV_EXPORTS BruteForceMatcher_OCL< L2<T> > : public BruteForceMatcher_OCL_base

        {

        public:

            explicit BruteForceMatcher_OCL() : BruteForceMatcher_OCL_base(L2Dist) {}

            explicit BruteForceMatcher_OCL(L2<T> /*d*/) : BruteForceMatcher_OCL_base(L2Dist) {}

        };

        template <> class CV_EXPORTS BruteForceMatcher_OCL< Hamming > : public BruteForceMatcher_OCL_base

        {

        public:

            explicit BruteForceMatcher_OCL() : BruteForceMatcher_OCL_base(HammingDist) {}

            explicit BruteForceMatcher_OCL(Hamming /*d*/) : BruteForceMatcher_OCL_base(HammingDist) {}

        };



        /////////////////////////////// PyrLKOpticalFlow /////////////////////////////////////

        class CV_EXPORTS PyrLKOpticalFlow

        {

        public:

            PyrLKOpticalFlow()

            {

                winSize = Size(21, 21);

                maxLevel = 3;

                iters = 30;

                derivLambda = 0.5;

                useInitialFlow = false;

                minEigThreshold = 1e-4f;

                getMinEigenVals = false;

                isDeviceArch11_ = false;

            }



            void sparse(const oclMat &prevImg, const oclMat &nextImg, const oclMat &prevPts, oclMat &nextPts,

                        oclMat &status, oclMat *err = 0);



            void dense(const oclMat &prevImg, const oclMat &nextImg, oclMat &u, oclMat &v, oclMat *err = 0);



            Size winSize;

            int maxLevel;

            int iters;

            double derivLambda;

            bool useInitialFlow;

            float minEigThreshold;

            bool getMinEigenVals;



            void releaseMemory()

            {

                dx_calcBuf_.release();

                dy_calcBuf_.release();



                prevPyr_.clear();

                nextPyr_.clear();



                dx_buf_.release();

                dy_buf_.release();

            }



        private:

            void calcSharrDeriv(const oclMat &src, oclMat &dx, oclMat &dy);



            void buildImagePyramid(const oclMat &img0, vector<oclMat> &pyr, bool withBorder);



            oclMat dx_calcBuf_;

            oclMat dy_calcBuf_;



            vector<oclMat> prevPyr_;

            vector<oclMat> nextPyr_;



            oclMat dx_buf_;

            oclMat dy_buf_;



            oclMat uPyr_[2];

            oclMat vPyr_[2];



            bool isDeviceArch11_;

        };
        //////////////// build warping maps ////////////////////
        //! builds plane warping maps
        CV_EXPORTS void buildWarpPlaneMaps(Size src_size, Rect dst_roi, const Mat &K, const Mat &R, const Mat &T, float scale, oclMat &map_x, oclMat &map_y);
        //! builds cylindrical warping maps
        CV_EXPORTS void buildWarpCylindricalMaps(Size src_size, Rect dst_roi, const Mat &K, const Mat &R, float scale, oclMat &map_x, oclMat &map_y);
        //! builds spherical warping maps
        CV_EXPORTS void buildWarpSphericalMaps(Size src_size, Rect dst_roi, const Mat &K, const Mat &R, float scale, oclMat &map_x, oclMat &map_y);
        //! builds Affine warping maps
        CV_EXPORTS void buildWarpAffineMaps(const Mat &M, bool inverse, Size dsize, oclMat &xmap, oclMat &ymap);

        //! builds Perspective warping maps
        CV_EXPORTS void buildWarpPerspectiveMaps(const Mat &M, bool inverse, Size dsize, oclMat &xmap, oclMat &ymap);

        ///////////////////////////////////// interpolate frames //////////////////////////////////////////////
        //! Interpolate frames (images) using provided optical flow (displacement field).
        //! frame0   - frame 0 (32-bit floating point images, single channel)
        //! frame1   - frame 1 (the same type and size)
        //! fu       - forward horizontal displacement
        //! fv       - forward vertical displacement
        //! bu       - backward horizontal displacement
        //! bv       - backward vertical displacement
        //! pos      - new frame position
        //! newFrame - new frame
        //! buf      - temporary buffer, will have width x 6*height size, CV_32FC1 type and contain 6 oclMat;
        //!            occlusion masks            0, occlusion masks            1,
        //!            interpolated forward flow  0, interpolated forward flow  1,
        //!            interpolated backward flow 0, interpolated backward flow 1
        //!
        CV_EXPORTS void interpolateFrames(const oclMat &frame0, const oclMat &frame1,
                                          const oclMat &fu, const oclMat &fv,
                                          const oclMat &bu, const oclMat &bv,
                                          float pos, oclMat &newFrame, oclMat &buf);

    }
}
#if defined _MSC_VER && _MSC_VER >= 1200
#  pragma warning( push)
#  pragma warning( disable: 4267)
#endif
#include "opencv2/ocl/matrix_operations.hpp"
#if defined _MSC_VER && _MSC_VER >= 1200
#  pragma warning( pop)
#endif

#endif /* __OPENCV_OCL_HPP__ */
