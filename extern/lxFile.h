#ifndef lxFile_h
#define lxFile_h

// Standard libraries
#ifndef LXDEPCHECK
#include <list>
#include <string>
#include <cstdio>
#endif  
//LXDEPCHECK - standart libraries


typedef char * lxFileBuff;

#ifdef THMSVC
#ifndef UINT32_MAX
# define UINT32_MAX (0xffffffffUL)
#endif
#ifndef uint32_t
#if (ULONG_MAX == UINT32_MAX) || defined (S_SPLINT_S)
  typedef unsigned long uint32_t;
# define UINT32_C(v) v ## UL
# ifndef PRINTF_INT32_MODIFIER
#  define PRINTF_INT32_MODIFIER "l"
# endif
#elif (UINT_MAX == UINT32_MAX)
  typedef unsigned int uint32_t;
# ifndef PRINTF_INT32_MODIFIER
#  define PRINTF_INT32_MODIFIER ""
# endif
# define UINT32_C(v) v ## U
#elif (USHRT_MAX == UINT32_MAX)
  typedef unsigned short uint32_t;
# define UINT32_C(v) ((unsigned short) (v))
# ifndef PRINTF_INT32_MODIFIER
#  define PRINTF_INT32_MODIFIER ""
# endif
#else
#error "Platform not supported"
#endif
#endif
#else
#include <stdint.h>
#endif

#define lxFileSizeT uint32_t


struct lxFileSize {
  lxFileSizeT m_size;
  operator lxFileSizeT & () {return this->m_size;}
  lxFileSizeT & operator = (const lxFileSizeT & right) {return this->m_size = right;}
  lxFileSizeT Save(lxFileBuff & ptr);
  lxFileSizeT Load(lxFileBuff & ptr);
};


struct lxFileDbl {
  double m_num;
  operator double & () {return this->m_num;}
  double & operator = (const double & right) {return this->m_num = right;}
  lxFileSizeT Save(lxFileBuff & ptr);
  lxFileSizeT Load(lxFileBuff & ptr);
};


struct lxFileDataPtr {
  lxFileSize m_position, m_size;

  lxFileDataPtr();
  void Clear();
  lxFileSizeT Save(lxFileBuff & ptr);
  lxFileSizeT Load(lxFileBuff & ptr);
};

struct lxFileData {
  
  void * m_data;
  lxFileSizeT m_size, m_buffSize;

  lxFileData();
  void Clear();
  void Copy(lxFileSizeT size, const void * src);
  void BuffResize(lxFileSizeT size);
  const void * GetData(lxFileDataPtr ptr);
  const char * GetString(lxFileDataPtr ptr);
  FILE * GetTmpFile(lxFileDataPtr ptr);
  lxFileDataPtr AppendStr(const char * str);
  lxFileDataPtr AppendData(const void * data, lxFileSizeT size);
  lxFileDataPtr AppendFile(const char * fnm);

};


struct lxFile3Point {
  lxFileDbl m_c[3];
  lxFileSizeT Save(lxFileBuff & ptr);
  lxFileSizeT Load(lxFileBuff & ptr);
};


struct lxFile3Angle {
  lxFileSize m_v[3];
  lxFileSizeT Save(lxFileBuff & ptr);
  lxFileSizeT Load(lxFileBuff & ptr);
};


struct lxFileSurvey {
  lxFileSize m_id, m_parent;
  lxFileDataPtr m_namePtr, m_titlePtr;
  lxFileSizeT Save(lxFileBuff & ptr);
  lxFileSizeT Load(lxFileBuff & ptr);
};


typedef std::list<lxFileSurvey> lxFileSurvey_list;


enum {
  LXFILE_STATION_FLAG_SURFACE = 1,
  LXFILE_STATION_FLAG_ENTRANCE = 2,
  LXFILE_STATION_FLAG_FIXED = 4,
  LXFILE_STATION_FLAG_CONTINUATION = 8,
  LXFILE_STATION_FLAG_HAS_WALLS = 16,
};


struct lxFileStation {
  lxFileSize m_id, m_surveyId;
  lxFileDataPtr m_namePtr, m_commentPtr;
  lxFileSize m_flags;
  lxFileDbl m_c[3];

  lxFileSizeT Save(lxFileBuff & ptr);
  lxFileSizeT Load(lxFileBuff & ptr);
  void SetFlag(int flag, bool value);
  bool GetFlag(int flag);
  lxFileStation();
};


typedef std::list<lxFileStation> lxFileStation_list;


enum {
  LXFILE_SHOT_FLAG_SURFACE = 1,
  LXFILE_SHOT_FLAG_DUPLICATE = 2,
  LXFILE_SHOT_FLAG_NOT_VISIBLE = 4,
  LXFILE_SHOT_FLAG_NOT_LRUD = 8,
  LXFILE_SHOT_FLAG_SPLAY = 8,
};


enum {
  LXFILE_SHOT_SECTION_NONE,
  LXFILE_SHOT_SECTION_OVAL,
  LXFILE_SHOT_SECTION_SQUARE, 
  LXFILE_SHOT_SECTION_DIAMOND,
  LXFILE_SHOT_SECTION_TUNNEL,
};


struct lxFileShot {
  lxFileSize m_from, m_to, m_surveyId;
  lxFileDbl m_fLRUD[4], m_tLRUD[4];
  lxFileDbl m_threshold;
  lxFileSize m_flags, m_sectionType;

  lxFileSizeT Save(lxFileBuff & ptr);
  lxFileSizeT Load(lxFileBuff & ptr);
  void SetFlag(int flag, bool value);
  bool GetFlag(int flag);
  lxFileShot();
};


typedef std::list<lxFileShot> lxFileShot_list;


struct lxFileScrap {
  lxFileSize m_id, m_surveyId;
  lxFileDataPtr m_pointsPtr, m_3AnglesPtr;
  lxFileSize m_numPoints, m_num3Angles;
  lxFileSizeT Save(lxFileBuff & ptr);
  lxFileSizeT Load(lxFileBuff & ptr);
};

typedef std::list<lxFileScrap> lxFileScrap_list;


struct lxFileSurface {
  lxFileSize m_id;
  lxFileSize m_width, m_height;
  lxFileDbl m_calib[6];
  lxFileDataPtr m_dataPtr;
  lxFileSizeT Save(lxFileBuff & ptr);
  lxFileSizeT Load(lxFileBuff & ptr);
};


typedef std::list<lxFileSurface> lxFileSurface_list;


enum {
  LXFILE_BITMAP_JPEG,
  LXFILE_BITMAP_PNG,
};

struct lxFileSurfaceBitmap {
  lxFileSize m_surfaceId;
  lxFileSize m_type;
  lxFileDbl m_calib[6];
  lxFileDataPtr m_dataPtr;
  lxFileSizeT Save(lxFileBuff & ptr);
  lxFileSizeT Load(lxFileBuff & ptr);
};


typedef std::list<lxFileSurfaceBitmap> lxFileSurfaceBitmap_list;


struct lxFile {

  static bool m_bigEndian;

  lxFileSurvey_list m_surveys;
  lxFileData m_surveysData;
  lxFileSizeT m_nSurveys;

  lxFileStation_list m_stations;
  lxFileData m_stationsData;
  lxFileSizeT m_nStations;

  lxFileShot_list m_shots;
  lxFileData m_shotsData;

  lxFileScrap_list m_scraps;
  lxFileData m_scrapsData;

  lxFileSurface_list m_surfaces;
  lxFileData m_surfacesData;

  lxFileSurfaceBitmap_list m_surfaceBitmaps;
  lxFileData m_surfaceBitmapsData;

  std::string m_error;
  FILE * m_file;

  static void switchEndian(char * data, lxFileSizeT size);
  
  void ImportLOX(const char * fn);
  void ExportLOX(const char * fn);

  void Import3D(const char * fn);
  void ImportPLT(const char * fn);

  lxFile();
  ~lxFile();

  void Clear();
  bool HasAnyWalls();
  void InterpolateMissingLRUD();
  lxFileSurvey * NewSurvey();
  lxFileStation * NewStation();
  lxFileShot * NewShot();

};

#endif


