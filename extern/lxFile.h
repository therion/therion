#ifndef lxFile_h
#define lxFile_h

// Standard libraries
#ifndef LXDEPCHECK
#include <list>
#include <string>
#include <cstdio>
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
#endif  
//LXDEPCHECK - standart libraries



typedef char * lxFileBuff;

struct lxFileSize {
  uint32_t m_size;
  operator uint32_t & () {return this->m_size;}
  uint32_t & operator = (const uint32_t & right) {return this->m_size = right;}
  uint32_t Save(lxFileBuff & ptr);
  uint32_t Load(lxFileBuff & ptr);
};


struct lxFileDbl {
  double m_num;
  operator double & () {return this->m_num;}
  double & operator = (const double & right) {return this->m_num = right;}
  uint32_t Save(lxFileBuff & ptr);
  uint32_t Load(lxFileBuff & ptr);
};


struct lxFileDataPtr {
  lxFileSize m_position, m_size;

  lxFileDataPtr();
  void Clear();
  uint32_t Save(lxFileBuff & ptr);
  uint32_t Load(lxFileBuff & ptr);
};

struct lxFileData {
  
  void * m_data;
  uint32_t m_size, m_buffSize;

  lxFileData();
  void Clear();
  void Copy(uint32_t size, const void * src);
  void BuffResize(uint32_t size);
  const void * GetData(lxFileDataPtr ptr);
  const char * GetString(lxFileDataPtr ptr);
  FILE * GetTmpFile(lxFileDataPtr ptr);
  lxFileDataPtr AppendStr(const char * str);
  lxFileDataPtr AppendData(const void * data, uint32_t size);
  lxFileDataPtr AppendFile(const char * fnm);

};


struct lxFile3Point {
  lxFileDbl m_c[3];
  uint32_t Save(lxFileBuff & ptr);
  uint32_t Load(lxFileBuff & ptr);
};


struct lxFile3Angle {
  lxFileSize m_v[3];
  uint32_t Save(lxFileBuff & ptr);
  uint32_t Load(lxFileBuff & ptr);
};


struct lxFileSurvey {
  lxFileSize m_id, m_parent;
  lxFileDataPtr m_namePtr, m_titlePtr;
  uint32_t Save(lxFileBuff & ptr);
  uint32_t Load(lxFileBuff & ptr);
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

  uint32_t Save(lxFileBuff & ptr);
  uint32_t Load(lxFileBuff & ptr);
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

  uint32_t Save(lxFileBuff & ptr);
  uint32_t Load(lxFileBuff & ptr);
  void SetFlag(int flag, bool value);
  bool GetFlag(int flag);
  lxFileShot();
};


typedef std::list<lxFileShot> lxFileShot_list;


struct lxFileScrap {
  lxFileSize m_id, m_surveyId;
  lxFileDataPtr m_pointsPtr, m_3AnglesPtr;
  lxFileSize m_numPoints, m_num3Angles;
  uint32_t Save(lxFileBuff & ptr);
  uint32_t Load(lxFileBuff & ptr);
};

typedef std::list<lxFileScrap> lxFileScrap_list;


struct lxFileSurface {
  lxFileSize m_id;
  lxFileSize m_width, m_height;
  lxFileDbl m_calib[6];
  lxFileDataPtr m_dataPtr;
  uint32_t Save(lxFileBuff & ptr);
  uint32_t Load(lxFileBuff & ptr);
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
  uint32_t Save(lxFileBuff & ptr);
  uint32_t Load(lxFileBuff & ptr);
};


typedef std::list<lxFileSurfaceBitmap> lxFileSurfaceBitmap_list;


struct lxFile {

  lxFileSurvey_list m_surveys;
  lxFileData m_surveysData;
  uint32_t m_nSurveys;

  lxFileStation_list m_stations;
  lxFileData m_stationsData;
  uint32_t m_nStations;

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


