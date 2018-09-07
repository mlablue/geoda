/**
 * GeoDa TM, Copyright (C) 2011-2015 by Luc Anselin - all rights reserved
 *
 * This file is part of GeoDa.
 * 
 * GeoDa is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GeoDa is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __GEODA_CENTER_OGR_LAYER_PROXY_H__
#define __GEODA_CENTER_OGR_LAYER_PROXY_H__

#include <sstream> 
#include <string>
#include <vector>
#include <ogrsf_frmts.h>
#include <wx/string.h>

// This is for Shapfile/DBF direct operation
#include "../DataViewer/TableInterface.h"
#include "../GdaShape.h"
#include "../GdaException.h"
#include "OGRFieldProxy.h"
#include "OGRLayerProxy.h"


using namespace std;

/**
 * A threaded proxy class for OGR layer. It will read all meta information, such
 * as field properties, and data from OGR data soruce.
 *
 * Note: OGR read data source row by row. But the data will be stored column
 * by column, so that it can be used by OGRTable and wxGrid easily.
 */
class OGRLayerProxy {
public:
	OGRLayerProxy(wxString layer_name,
                  OGRLayer* _layer,
                  GdaConst::DataSourceType _ds_type,
                  bool isNew=false);
    
    OGRLayerProxy(OGRLayer* _layer,
                  GdaConst::DataSourceType _ds_type,
                  OGRwkbGeometryType eGType,
                  int _n_rows = 0);
    
	~OGRLayerProxy();
	
    GdaConst::DataSourceType ds_type;
	ostringstream error_message;
    
	// progress indicator: -1 means error, otherwise means progress
	int         load_progress;
	bool        stop_reading;
	int         export_progress;
	bool        stop_exporting;
	
    
    bool        is_writable;
	wxString    name;
	int			n_rows;
	int			n_cols;
	OGRLayer*	layer;
    
    //!< Geometry type of OGRLayer
    OGRwkbGeometryType eLayerType;
    
    //!< Fields and the meta data are stored in OGRFieldProxy.
	vector<OGRFieldProxy*> fields;
    
    //!< OGR will read data sources and store the he content in many OGRFeature
    //!< objects. The OGRLayerProxy will maintain these objects until the proxy
    //!< is dismissed.
	vector<OGRFeature*> data;
    
    //!< OGR layer GeomType
    OGRwkbGeometryType eGType;
    
    OGRGeometry*       mapContour;
    
	static OGRFieldType GetOGRFieldType(GdaConst::FieldType field_type);
    
    void SetOGRLayer(OGRLayer* new_layer);
    
    bool HasError();
    
    bool GetExtent(double& minx, double& miny, double& maxx, double& maxy);
    
    OGRwkbGeometryType GetShapeType();
    
    OGRLayer* GetOGRLayer();
    
    int GetNumRecords();
    
    int GetNumFields();
    
    OGRSpatialReference* GetSpatialReference();
    
    void ApplyProjection(OGRCoordinateTransformation* poCT);
    
	/**
	 * Save() function tries to save any changes to original data source.
	 * It may return failure because the layer doesn't support writeback.
	 */
	void Save();
	
    /**
	 * Export current ogr layer to  layer in other ogr data source
	 * @param format exported driver name (OGR style)
	 * @param dest_datasource exported data source name (OGR style)
	 */
	void Export(wxString format, wxString dest_datasource, wxString new_layer_name, bool is_update);
    
	void T_Export(wxString format, wxString dest_datasource, wxString new_layer_name, bool is_update);
    
	void T_StopExport();

    /**
     * Add new features to an empty OGRLayer
     * This function should be only used when create a new OGRLayer
     */
    void AddFeatures(vector<OGRGeometry*>& geometries,
                     TableInterface* table,
                     vector<int>& selected_rows);
	/**
	 * Read geometries and save to Shapefile::Main data structure.
	 */
	bool ReadGeometries(Shapefile::Main& p_main);
    
    bool AddGeometries(Shapefile::Main& p_main);
    
    void GetCentroids(vector<GdaPoint*>& centroids);
    
    static GdaPolygon* OGRGeomToGdaShape(OGRGeometry* geom);
    static GdaPolygon* GetMapBoundary(vector<OGRGeometry*>& geoms);

    GdaPolygon* GetMapBoundary();
    
    Shapefile::ShapeType GetGdaGeometries(vector<GdaShape*>& geoms,
                                          OGRSpatialReference* input_sr=NULL);
    
    Shapefile::ShapeType GetOGRGeometries(vector<OGRGeometry*>& geoms,
                                          OGRSpatialReference* input_sr=NULL);
    
	/**
	 * Read table data from ogr OGRFeatures.
	 * Note: Geometries are saved as raw "wkb" format. Developer needs to call
	 * ReadGeometries() function to retrieve/phrase geometries into memory.
	 */
	bool ReadData();
    
    /**
     * Get OGRFieldProxy by an in put field position
     */
    OGRFieldProxy* GetField(int pos);
    
    /**
     * Get OGRFieldProxy by a input field name.
     */
    OGRFieldProxy* GetField(const wxString& field_name);
    
	/**
	 * AddField() function tries to add a new field in OGRLayer.
	 * NOTE: this may has no impact on orignal data source. It only works in 
	 * memory. If ogr data source doesn't support add field, user can "export"
	 * the new field(s) in memory to a new file.
	 */
	int AddField(const wxString& field_name,
                 GdaConst::FieldType field_type,
				 int field_length, int field_precision);
	/**
	 *
	 */
	void UpdateFieldProperties(int col);
    
    /**
	 * Get field name by an input field position.
	 */
	wxString GetFieldName(int pos);
    
    /**
	 * Set field name at an input field position.
	 */
    void SetFieldName(int pos, const wxString& new_fname);
    
	/**
	 *
	 */
	void DeleteField(int pos);
    
	/**
	 *
	 */
	void DeleteField(const wxString& field_name);
    
	/**
	 *
	 */
	int GetFieldPos(const wxString& field_name);
    
    /**
     */
	GdaConst::FieldType GetFieldType(int pos);
    
    /**
     */
    GdaConst::FieldType GetFieldType(const wxString& field_name);
	/**
	 *
	 */
	int GetFieldLength(int pos);
    void SetFieldLength(int pos, int new_len);
	/**
	 *
	 */
	int GetFieldDecimals(int pos);
    void SetFieldDecimals(int pos, int new_dec);
	/**
	 *
	 */
	bool UpdateColumn();
    bool UpdateColumn(int col_idx, vector<double> &vals);
    bool UpdateColumn(int col_idx, vector<wxInt64> &vals);
    bool UpdateColumn(int col_idx, vector<wxString> &vals);
	/**
	 *
	 */
	bool IsTableOnly();
	/**
	 *
	 */
	bool UpdateOGRFeature(OGRFeature* feature);
	/**
	 *
	 */
	bool AppendOGRFeature(vector<wxString>& content);
	/**
	 *
	 */
	bool InsertOGRFeature();
	
	/**
	 * var_list:  variable/column/field list
	 * var_type_map: variable/column/field -- field type
	 */
	void GetVarTypeMap(vector<wxString>& var_list,
					   map<wxString, GdaConst::FieldType>& var_type_map);
	/**
	 *
	 */
    OGRFeature* GetFeatureAt(int rid);
    
    OGRGeometry* GetGeometry(int idx);
    
    vector<wxString> GetIntegerFieldNames();
    
    bool IsUndefined(int rid, int cid);
    
    wxString GetValueAt(int rid, int cid);
    
    void GetValueAt(int rid, int cid, GIntBig* val);
    
    void GetValueAt(int rid, int cid, double* val);
    
    void SetValueAt(int rid, int cid, GIntBig val, bool undef=false);
    
    void SetValueAt(int rid, int cid, double val, bool undef=false);
    
    void SetValueAt(int rid, int cid, int year, int month, int day, bool undef=false);
    
    void SetValueAt(int rid, int cid, int year, int month, int day, int hour, int minute, int second, bool undef=false);
    
    void SetValueAt(int rid, int cid, const char* val, bool is_new=true, bool undef=false);
    
protected:
    OGRFeatureDefn* featureDefn;
    
    OGRSpatialReference* spatialRef;
    
    void GetExtent(Shapefile::Main& p_main, Shapefile::PointContents* pc, int row_idx);
    
    void GetExtent(Shapefile::Main& p_main, Shapefile::PolygonContents* pc, int row_idx);
    
    void CopyEnvelope(OGRPolygon* p, Shapefile::PolygonContents* pc);
	
    /**
	 * Read field information and save to OGRFieldProxy array.
	 */
	bool ReadFieldInfo();
    
	bool IsFieldExisted(const wxString& field_name);
    
    bool CallCartoDBAPI(wxString url);
};

#endif
