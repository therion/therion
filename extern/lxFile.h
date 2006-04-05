#ifndef lxFile_h
#define lxFile_h

// Standard libraries
#ifndef LXDEPCHECK
#include <list>
#include <string>
#include <cstdio>
#endif  
//LXDEPCHECK - standart libraries

typedef size_t lxFileSize;
typedef double lxFileDbl;


struct lxFileDataPtr {
  lxFileSize m_position, m_size;

  lxFileDataPtr();
  void Clear();
};

struct lxFileData {
  
  void * m_data;
  size_t m_size, m_buffSize;

  lxFileData();
  void Clear();
  void Copy(size_t size, const void * src);
  void BuffResize(size_t size);
  const void * GetData(lxFileDataPtr ptr);
  const char * GetString(lxFileDataPtr ptr);
  FILE * GetTmpFile(lxFileDataPtr ptr);
  lxFileDataPtr AppendStr(const char * str);
  lxFileDataPtr AppendData(const void * data, size_t size);
  lxFileDataPtr AppendFile(const char * fnm);

};


struct lxFile3Point {
  lxFileDbl m_c[3];
};


struct lxFile3Angle {
  lxFileSize m_v[3];
};


struct lxFileSurvey {
  lxFileSize m_id, m_parent;
  lxFileDataPtr m_namePtr, m_titlePtr;
};


typedef std::list<lxFileSurvey> lxFileSurvey_list;


enum {
  LXFILE_STATION_FLAG_SURFACE = 1,
  LXFILE_STATION_FLAG_ENTRANCE = 2,
  LXFILE_STATION_FLAG_FIXED = 4,
  LXFILE_STATION_FLAG_CONTINUATION = 8,
};


struct lxFileStation {
  lxFileSize m_id, m_surveyId;
  lxFileDataPtr m_namePtr, m_commentPtr;
  lxFileSize m_flags;
  lxFileDbl m_c[3];

  lxFileStation();
};


typedef std::list<lxFileStation> lxFileStation_list;


enum {
  LXFILE_SHOT_FLAG_SURFACE = 1,
  LXFILE_SHOT_FLAG_DUPLICATE = 2,
};


enum {
  LXFILE_SHOT_SECTION_NONE,
  LXFILE_SHOT_SECTION_OVAL,
  LXFILE_SHOT_SECTION_SQARE, 
  LXFILE_SHOT_SECTION_DIAMOND,
  LXFILE_SHOT_SECTION_TUNNEL,
};


struct lxFileShot {
  lxFileSize m_from, m_to, m_surveyId;
  lxFileDbl m_fLRUD[4], m_tLRUD[4];
  lxFileDbl m_threshold;
  lxFileSize m_flags, m_sectionType;

  lxFileShot();
};


typedef std::list<lxFileShot> lxFileShot_list;


struct lxFileScrap {
  lxFileSize m_id, m_surveyId;
  lxFileDataPtr m_pointsPtr, m_3AnglesPtr;
  lxFileSize m_numPoints, m_num3Angles;
};

typedef std::list<lxFileScrap> lxFileScrap_list;


struct lxFileSurface {
  lxFileSize m_id;
  lxFileSize m_width, m_height;
  lxFileDbl m_calib[6];
  lxFileDataPtr m_dataPtr;
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
};


typedef std::list<lxFileSurfaceBitmap> lxFileSurfaceBitmap_list;


struct lxFile {

  lxFileSurvey_list m_surveys;
  lxFileData m_surveysData;

  lxFileStation_list m_stations;
  lxFileData m_stationsData;

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
  void ImportTRO(const char * fn);

  lxFile();
  ~lxFile();
  void Clear();

};

#endif


