
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <list>
#include <functional>
#include <stdexcept>

#include "comparators.h"

extern "C" {
#include "strcmp95.h"
}

using std::list;


int
is_abbreviation(const char * s1, const char * s2) {

    int cnt = 0;
    while ( *s1 != '\0' && *s2 != '\0' && *s1 == *s2 ) {
        ++s1;
        ++s2;
        ++cnt;
    }
    if ( *s1 != '\0' && *s2 != '\0' )
        return 0;
    //else if ( *s1 == '\0' && *s2 == '\0' )
    //    return 2;
    else
        return cnt;
}


int
is_misspell( const char * s1, const char * s2 ) {
    const int size_diff = strlen(s1)- strlen(s2);
    const char * plong = NULL, *pshort = NULL;
    int hit = 0;


    if ( size_diff == 1 || size_diff == -1  ) {
        // one character is missing
        if ( size_diff == 1 ) {
            plong = s1;
            pshort = s2;
        }
        else {
            plong = s2;
            pshort = s1;
        }

        while ( *pshort != '\0' ) {
            if ( *plong++ != *pshort++ ) {
                if ( hit )
                    return 0;
                ++plong;
                hit = 1;
            }
        }

        if ( *pshort != '\0' && *plong != '\0' )
            return 0;
        else
            return 1;
    }

    else if ( size_diff == 0) {
        //switch or misspell
        while ( *s1 != '\0' ) {
            if ( *s1 != *s2 ) {
                if ( hit )
                    return 0;
                else {
                    hit = 1;
                    plong = s1;
                    pshort = s2;
                    ++s1;
                    ++s2;
                    if ( *s1 == '\0' )
                        return 3; //misspelling of last char
                }

            }
            ++s1;
            ++s2;
        }
        if ( hit == 0 )
            return 4; //exact match
        else {
            if ( *plong != *pshort && *( plong + 1 ) == *(pshort + 1) )
                return 3; //misspelling
            else if ( *plong == * (pshort + 1) && *pshort == *(plong + 1 ))
                return 2; //switch of 2 chars
            else
                return 0;
        }
    }
    else
        return 0;

}


char *
extract_initials(char * dest, const char * source) {

    if ( source == NULL || dest == NULL )
        return NULL;
    char * ret = dest;
    static const char delim = ' ';
    while ( *source != '\0') {
        while ( *source == delim )
            ++source;
        *dest++ = *source;
        while ( *source != delim && *source != '\0' )
            ++source;
    }
    *dest = '\0';
    return ret;
};


int
nospacecmp(const char* str1, const char* str2) {

    const char *c1, *c2;
    const char delim = ' ';
    for(c1 = str1, c2=str2; (*c1 != '\0') && (*c2 != '\0'); ++c1, ++c2 ){
        while (*c1 == delim ) ++c1;
        while (*c2 == delim ) ++c2;
        if(*c1!=*c2){
            return -1 + (*c1 > *c2)*2;
        }
    }
    return  ( *c1!='\0' ) - ( *c2!='\0' );
}


int
jwcmp(const string & str1, const string& str2) {

    if ( str1.empty() || str2.empty() )
        return 0;

    double cmpres = strcmp95_modified(str1.c_str(), str2.c_str());
    register int score = 0;
    if ( cmpres > 0.7 )
        ++score;
    if ( cmpres > 0.8 )
        ++score;
    if ( cmpres > 0.9 )
        ++score;
    if ( cmpres > 0.95 )
        ++score;
    if ( cmpres > 0.99 )
        ++score;

    return score;
}


int
midnamecmp (const string & s1, const string & s2) {

    if (s1.empty() && s2.empty())
      return 2;

    if (s1.empty() || s2.empty())
        return 1;

    const char * p1 = s1.c_str();
    const char * p2 = s2.c_str();

    while ( *p1 != '\0' && *p2 != '\0') {
        if ( *p1++ != * p2++)
            return 0;
    }
    return 3;
}


int
distcmp(const string & inputlat1, const string & inputlon1, const string & inputctry1, const char * inputstreet1,
        const string & inputlat2, const string & inputlon2, const string & inputctry2, const char * inputstreet2) {

    /*
//    printf("DISTCOMP:\n");
    // Extreme points of contiguous 48
    double northernmost=4938;
    double southernmost=2454;
    double easternmost=-6695;
    double westernmost=-12451;

    double dist;
    int streetmatch;

    latlon *ll1 = (latlon*)latlon1, *ll2 = (latlon*)latlon2;
    int missing = ((abs(ll1->lat)<0.0001 && abs(ll1->lon)< 0.0001) || (abs(ll2->lat)<0.0001 && abs(ll2->lon)<0.0001));
    int in_us = ll1->lat < northernmost && ll1->lat > southernmost &&
                ll1->lon < easternmost && ll1->lon > westernmost &&
                ll2->lat < northernmost && ll2->lat > southernmost &&
                ll2->lon < easternmost && ll1->lon > westernmost;
    size = size;

    dist = 3963.0 * acos(sin(ll1->lat/DEG2RAD) * sin(ll2->lat/DEG2RAD) + cos(ll1->lat/DEG2RAD) * cos(ll2->lat/DEG2RAD) *  cos(ll2->lon/DEG2RAD -ll1->lon/DEG2RAD));
    //if(dist > 1){
        //printf("\targs: %f, %f ; %f, %f\n", ll1->lat, ll1->lon, ll2->lat, ll2->lon);
        //printf("\traw: %f\n", dist);
    //}
    streetmatch = (((latlon*)latlon1)->street!=NULL) &&
                  (((latlon*)latlon2)->street!=NULL) &&
                  (((latlon*)latlon1)->street[0] != '\0')&&
                  (((latlon*)latlon2)->street[0] != '\0')&&
                  (strcmp(((latlon*)latlon1)->street, ((latlon*)latlon2)->street)==0);

    return missing +
           in_us ?
               2*(dist < 100) + (dist < 75) + (dist < 50) + 2*(dist < 10) +
                   ((dist < 1) &&  streetmatch):
               2*(dist < 100) + (dist < 75) + (dist < 50) + (dist < 10);
    */

    static const double R = 3963.0; //radius of the earth is 6378.1km = 3963 miles
    static const double DEG2RAD = 5729.58;
    static const double northernmost = 4938;
    static const double southernmost = 2454;
    static const double easternmost = -6695;
    static const double westernmost = -12451;

    const double lat1 = atof(inputlat1.c_str());
    const double lon1 = atof(inputlon1.c_str());
    const double lat2 = atof(inputlat2.c_str());
    const double lon2 = atof(inputlon2.c_str());

    const double missing_val = 0.0001;
    int missing = ( ( fabs(lat1) < missing_val && fabs(lon1) < missing_val ) ||
                    ( fabs(lat2) < missing_val && fabs(lon2) < missing_val) ) ? 1 : 0;
    int in_us = (     lat1 < northernmost && lat1 > southernmost &&
                    lon1 < easternmost && lon1 > westernmost &&
                    lat2 < northernmost && lat2 > southernmost &&
                    lon2 < easternmost && lon2 > westernmost ) ? 1 : 0;

    const double radlat1 = lat1/DEG2RAD;
    const double radlon1 = lon1/DEG2RAD;
    const double radlat2 = lat2/DEG2RAD;
    const double radlon2 = lon2/DEG2RAD;

    const double cos_lat1 = cos(radlat1);
    const double cos_lat2 = cos(radlat2);
    const double cos_lon1 = cos(radlon1);
    const double cos_lon2 = cos(radlon2);
    const double sin_lon1 = sin(radlon1);
    const double sin_lon2 = sin(radlon2);


    // R=radius, theta = colatitude, phi = longitude
    // Spherical coordinate -> Cartesian coordinate:
    // x=R*sin(theta)*cos(phi) = R*cos(latitude)*cos(longitude)
    // y = R*sin(theta)*sin(phi) = R*cos(latitude)*sin(longitude)
    // z = R*cos(phi) = R * cos(longitude)
    // Cartesion distance = sqrt( ( x1-x2)^2 + (y1-y2)^2 + (z1 - z2)^2 );
    // Spherical distance = arccos( 1 - (Cartesian distance)^2 / (2*R^2) ) * R;

    const double x1 = cos_lat1 * cos_lon1;
    const double x2 = cos_lat2 * cos_lon2;
    const double y1 = cos_lat1 * sin_lon1;
    const double y2 = cos_lat2 * sin_lon2;
    const double z1 = cos_lon1;
    const double z2 = cos_lon2;

    const double cart_dist = sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2) );
    const double dist = acos(1 - cart_dist*cart_dist / 2 ) * R;

    int streetmatch = ( strlen(inputstreet1) != 0 && strlen( inputstreet2) != 0 &&
                  (strcmp(inputstreet1, inputstreet2) == 0 )) ? 1 : 0;

    return missing +
           in_us ?
               2*(dist < 100) + (dist < 50) + 2*(dist < 10) +
                   ((dist < 1) &&  streetmatch):
               2*(dist < 100) + (dist < 50) + (dist < 10);

}

/**
 * This function should take float arguments, because floats are
 * supplied in the schema. Somewhere, they are being turned into
 * strings, then have to be turned back into floats here.
 * Also, 0 lat, 0 lon is a viable location, in the Gulf of
 * Guinea in fact. This function should not imply that
 * the distance is "missing." Also, given this is computing a
 * distance, having anything other than zero returned for
 * 0 distance is really disturbing.
 */
int
latloncmp(const string & inputlat1, const string & inputlon1,
          const string & inputlat2, const string & inputlon2 ) {

    static const double R = 3963.0; //radius of the earth is 6378.1km = 3963 miles
    static const double pi = 3.1415926;
    //rad = degree * pi / 180
    static const double DEG2RAD = pi / 180 ;

    const double lat1 = atof(inputlat1.c_str());
    const double lon1 = atof(inputlon1.c_str());
    const double lat2 = atof(inputlat2.c_str());
    const double lon2 = atof(inputlon2.c_str());

    const double missing_val = 0.0001;
    int missing = ( ( fabs(lat1) < missing_val && fabs(lon1) < missing_val ) ||
                    ( fabs(lat2) < missing_val && fabs(lon2) < missing_val) ) ? 1 : 0;

    if (missing) return 1;

    const double radlat1 = lat1 * DEG2RAD;
    const double radlon1 = lon1 * DEG2RAD;
    const double radlat2 = lat2 * DEG2RAD;
    const double radlon2 = lon2 * DEG2RAD;

    const double cos_lat1 = cos(radlat1);
    const double cos_lat2 = cos(radlat2);
    const double cos_lon1 = cos(radlon1);
    const double cos_lon2 = cos(radlon2);
    const double sin_lon1 = sin(radlon1);
    const double sin_lon2 = sin(radlon2);
    const double sin_lat1 = sin(radlat1);
    const double sin_lat2 = sin(radlat2);


    // R=radius, theta = colatitude, phi = longitude
    // Spherical coordinate -> Cartesian coordinate:
    // x=R*sin(phi)*cos(theta) = R*cos(latitude)*cos(longitude)
    // y = R*sin(phi)*sin(theta) = R*cos(latitude)*sin(longitude)
    // z = R*cos(phi) = R * sin(latitude)
    // Cartesion distance = sqrt( ( x1-x2)^2 + (y1-y2)^2 + (z1 - z2)^2 );
    // Spherical distance = arccos( 1 - (Cartesian distance)^2 / (2*R^2) ) * R;

    const double x1 = cos_lat1 * cos_lon1;
    const double x2 = cos_lat2 * cos_lon2;
    const double y1 = cos_lat1 * sin_lon1;
    const double y2 = cos_lat2 * sin_lon2;
    const double z1 = sin_lat1;
    const double z2 = sin_lat2;

    const double cart_dist_sq = (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2);
    const double dist = acos(1 - cart_dist_sq / 2) * R;


    if ( dist < 1.0 )
        return 5;
    else if ( dist < 10 )
        return 4;
    else if ( dist < 25)
        return 3;
    else if ( dist < 50 )
        return 2;
    else
        return 1;
}


int
streetcmp(const string & inputstreet1, const string & inputstreet2) {

    int streetmatch = ( inputstreet1.size() != 0 && inputstreet2.size() != 0 
        && (inputstreet1 == inputstreet2 )) ? 1 : 0;
    return streetmatch;
}


int
countrycmp(const string & country1, const string & country2 ) {

    static const string US_label ("US");
    int score = 0;
    if ( country1 == country2 ) {
        ++score;
        if ( country1 == US_label)
            ++score;
    }
    return score;
}


int
classcmp(const string & class1, const string & class2) {

    return (class1 == class2 ) ? 1 : 0;
}


int
coauthorcmp(const string & coauthor1, const string & coauthor2) {

    return (coauthor1 == coauthor2) ? 1 : 0;
}


int
asgcmp (const string & s1, const string &s2) {

    if (s1.empty() || s2.empty()) return 1;

    double cmpres = strcmp95_modified(s1.c_str(), s2.c_str());

    if (cmpres > 0.9)
        return 4;
    else if (cmpres > 0.8)
        return 3;
    else if (cmpres > 0.7)
        return 2;
    else
        return 0;
}


int
name_compare(const string & s1,
             const string & s2,
             const unsigned int prev,
             const unsigned int cur) {

    if (s1.empty() || s2.empty()) return 1;
    if (s1 == s2) return 4;

    int misspell_score = is_misspell(s1.c_str(), s2.c_str()) ;
    if (misspell_score) return 3;

    unsigned int abbrev_score = is_abbreviation (s1.c_str(), s2.c_str());
    if (abbrev_score == 0) {
        return 0;
    } else if ( cur != 0 && cur <= abbrev_score ) {
        if ( prev == 0 || ( prev != 0 && prev > abbrev_score ) ) return 4;
    }

    return 2;
}
