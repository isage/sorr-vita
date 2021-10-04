 /*
 *  Copyright (c) 2010-2011 Prg orlando8nov@yahoo.com.mx
 *
 *  This file is part of mod_chipmunk, a Chipmunk physics binding for BennuGD.
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty. In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *     1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *
 *     2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *
 *     3. This notice may not be removed or altered from any source
 *     distribution.
 *
 */

/*
 *  Thanks to SplinterGU, Josebita & FreeYourMind
 */

#ifndef __MODCHIPMUNK_SYMBOLS_H
#define __MODCHIPMUNK_SYMBOLS_H

#include <chipmunk.h>
#include <bgddl.h>

#ifdef __BGDC__
char __bgdexport( mod_chipmunk, globals_def )[] =
    "STRUCT gphysics\n"
    "int space;\n"
    "float gravity_X;\n"                                                  /* Access and set with *(FLOAT *)GLOADDR(mod_chipmunk,GLO_GRAVITY_X)) */
    "float gravity_Y;\n"                                                  /* Access and set with *(FLOAT *)GLOADDR(mod_chipmunk,GLO_GRAVITY_Y)) */
    "float bias_coef=0.5;\n"                                                  /* Access and set with *(FLOAT *)GLOADDR(mod_chipmunk,GLO_BIAS_COEF)) */
    "float collision_slop=0.1;\n"                                             /* Access and set with *(FLOAT *)GLOADDR(mod_chipmunk,GLO_COLLISION_SLOP)) */
    "int contact_persistence=3;\n"
    "int iterations=10;\n"                                        /* Access and set with GLOADDR(mod_chipmunk,GLO_CONTACT_PERSISTENCE)) */
    "float damping=1.0;\n"
    "float idleSpeedThreshold =0;\n"
    "float sleepTimeThreshold =9999999999;\n"
    "float interval =1.0/25.0;\n"
    "int phresolution =3;\n"
    "float distPercent =0.5;\n"
    "END\n"
    "cpVect cpvzero=0,0;\n"

    ;

char __bgdexport( mod_chipmunk, locals_def )[] =
    "STRUCT lphysics\n"
    "int body=0;\n"                                                        /* Access and set with GLODWORD(mod_chipmunk,GLO_SPACE) */
    "int shape=0;\n"                                                        /* Access and set with GLODWORD(mod_chipmunk,GLO_SPACE) */
    "int incr_x=0;\n"                                    /* Access and set with GLOADDR(mod_chipmunk,GLO_CONTACT_PERSISTENCE)) */
    "int incr_y=0;\n"
    "float inertia=1.0;\n"
    "float mass=1;\n"
    "int static=0;\n"
    "float elasticity=1.0;\n"
    "float friction=1.0;\n"
    "int group=CP_NO_GROUP;\n"
    "int layers=CP_ALL_LAYERS;\n"
    "int ShapeType=TYPE_NONE;\n"
    "int * params=null;\n"
    "int collisionType=0;\n"
    "int sensor=0;\n"
    "end\n"

    ;

char __bgdexport( mod_chipmunk, types_def )[] =
    "TYPE cpVect\n"
        " float x;\n"
        " float y;\n"
    "END\n"

    "TYPE cpBB\n"
        " float l, b, r ,t;\n"
    "END\n"

    "TYPE cpsegmentqueryinfo\n"
        "int shape, id;\n"
        "float x,y,t; "
        "cpVect n; \n"
    "END\n"

    "TYPE tPoints\n"
        " cpVect Point, normal;\n"
        " float dist;"
    " end \n"

    "TYPE cpContactPointSet\n"
        "int count;\n"
        "int id1, id2;\n"
        "int shape1,shape2;\n"
        "tPoints points[6]; "
    "end\n"

    "TYPE WaterS\n "
      "int*ids=0;\n"
      "int size=0;\n"
      "float kNorm=2.2104;\n"
      "float kNearNorm=3.3157;\n"
      "float kRestDensity=82;\n"
      "float kStiffness=0.08;\n"
      "float kNearStiffness=0.1;\n"
      "float kSurfaceTension=0.0004;\n"
      "float kLinearViscocity=0.5;\n"
      "float kQuadraticViscocity=1;\n"
      "float KH=120;\n"
    "end\n"
//"TYPE cpSegmentQueryInfo\n" " cpShape *shape;\n " " float t;\n " "cpVect n;\n""END\n"
    ;

DLCONSTANT __bgdexport( mod_chipmunk, constants_def )[] =
{
    { "CP_C_ITERATIONS", TYPE_INT, CP_C_ITERATIONS },
    { "CP_C_GRAVITY", TYPE_INT, CP_C_GRAVITY },
    { "CP_C_DAMPING", TYPE_INT, CP_C_DAMPING },
    { "CP_C_IDLESPEEDTHRESHOLD", TYPE_INT, CP_C_IDLESPEEDTHRESHOLD },
    { "CP_C_SLEEPTIMETHRESHOLD", TYPE_INT, CP_C_SLEEPTIMETHRESHOLD },
    { "CP_C_staticBody", TYPE_INT, CP_C_staticBody },

    { "CP_C_M", TYPE_INT, CP_C_M },
    { "CP_C_I", TYPE_INT, CP_C_I },
    { "CP_C_P", TYPE_INT, CP_C_P },
    { "CP_C_V", TYPE_INT, CP_C_V },
    { "CP_C_F", TYPE_INT, CP_C_F },
    { "CP_C_A", TYPE_INT, CP_C_A },
    { "CP_C_W", TYPE_INT, CP_C_W },
    { "CP_C_T", TYPE_INT, CP_C_T },
    { "CP_C_ROT", TYPE_INT, CP_C_ROT },
    { "CP_C_V_LIMIT", TYPE_INT, CP_C_V_LIMIT },
    { "CP_C_W_LIMIT", TYPE_INT, CP_C_W_LIMIT },

    { "CP_C_BODY", TYPE_INT, CP_C_BODY },
    { "CP_C_BB", TYPE_INT, CP_C_BB },
    { "CP_C_E", TYPE_INT, CP_C_E },
    { "CP_C_U", TYPE_INT, CP_C_U },
    { "CP_C_SENSOR", TYPE_INT, CP_C_SENSOR },
    { "CP_C_SURFACE_V", TYPE_INT, CP_C_SURFACE_V },
    { "CP_C_COLLISION_TYPE", TYPE_INT, CP_C_COLLISION_TYPE },
    { "CP_C_GROUP", TYPE_INT, CP_C_GROUP },
    { "CP_C_LAYERS", TYPE_INT, CP_C_LAYERS },
    { "CP_C_DATA", TYPE_INT, CP_C_DATA },
//    { "INFINITY", TYPE_FLOAT, 10000000.0 },
    { "NOT_GRABABLE_MASK", TYPE_INT, NOT_GRABABLE_MASK },
    { "GRABABLE_MASK_BIT", TYPE_INT, GRABABLE_MASK_BIT },
    { "CP_NO_GROUP", TYPE_INT, CP_NO_GROUP },

    { "CP_C_MAXFORCE", TYPE_INT, CP_C_MAXFORCE },
    { "CP_C_BIASCOEF", TYPE_INT, CP_C_BIASCOEF },
    { "CP_C_MAXBIAS", TYPE_INT, CP_C_MAXBIAS },
    { "CP_C_CA", TYPE_INT, CP_C_CA },
    { "CP_C_CB", TYPE_INT, CP_C_CB },

    { "CP_ALL_LAYERS", TYPE_INT, CP_ALL_LAYERS },

    { "TYPE_NONE", TYPE_INT, TYPE_NONE },
    { "TYPE_BOX", TYPE_INT, TYPE_BOX },
    { "TYPE_CIRCLE", TYPE_INT, TYPE_CIRCLE },
    { "TYPE_CONVEX_POLYGON", TYPE_INT, TYPE_CONVEX_POLYGON },
    { "TYPE_LINE", TYPE_INT, TYPE_LINE },
    { "TYPE_EMPTY", TYPE_INT, TYPE_EMPTY },

    { "CP_C_ANCHR1", TYPE_INT, CP_C_ANCHR1 },
    { "CP_C_ANCHR2", TYPE_INT, CP_C_ANCHR2 },
    { "CP_C_DIST", TYPE_INT, CP_C_DIST },
    { "CP_C_MIN", TYPE_INT, CP_C_MIN },
    { "CP_C_MAX", TYPE_INT, CP_C_MAX },
    { "CP_C_GROOVEB", TYPE_INT, CP_C_GROOVEB },
    { "CP_C_GROOVEA", TYPE_INT, CP_C_GROOVEA },
    { "CP_C_RESTLENGTH", TYPE_INT, CP_C_RESTLENGTH },
    { "CP_C_STIFFNESS", TYPE_INT, CP_C_STIFFNESS },
    { "CP_C_DAMPING", TYPE_INT, CP_C_DAMPING },
    { "CP_C_RESTANGLE", TYPE_INT, CP_C_RESTANGLE },
    { "CP_C_ANGLE", TYPE_INT, CP_C_ANGLE },
    { "CP_C_PHASE", TYPE_INT, CP_C_PHASE },
    { "CP_C_RATCHET", TYPE_INT, CP_C_RATCHET },
    { "CP_C_RATIO", TYPE_INT, CP_C_RATIO },
    { "CP_C_RATE", TYPE_INT, CP_C_RATE },

    { "CM_PI", TYPE_INT, CM_PI },

    { NULL , 0 , 0 }
} ;

char * __bgdexport( mod_chipmunk, modules_dependency )[] =
{
    "librender",
    "libgrbase",
    "libdraw",
    "libblit",
    NULL
};

DLSYSFUNCS __bgdexport( mod_chipmunk, functions_exports) [] =
{
    {"INFINITYF"                            , ""             , TYPE_FLOAT     , 0},
    {"modChipmunkdeg2rad"                   , "F"            , TYPE_FLOAT     , 0},
    {"SEGMENTQUERYHITDIST"                  , "IIIIP"        , TYPE_FLOAT     , 0},
    {"SEGMENTQUERYHITPOINT"                 , "IIIIPPP"      , TYPE_INT       , 0},
    {"FORCECREATEBODY"                      , ""             , TYPE_INT       , 0},
    {"FORCECREATEBODY"                      , "I"            , TYPE_INT       , 0},
    {"CLEANSPACE"                           , ""             , TYPE_INT       , 0},
    {"CPFLERP"                              , "FFF"          , TYPE_FLOAT     , 0},
    {"CPFCLAMP"                             , "FFF"          , TYPE_FLOAT     , 0},
    {"CPFLERPCONST"                         , "FFF"          , TYPE_FLOAT     , 0},
    {"CPVEQL"                               , "PP"           , TYPE_INT       , 0},
    {"CPVADD"                               , "PPP"          , TYPE_INT       , 0},
    {"CPVSUB"                               , "PPP"          , TYPE_INT       , 0},
    {"CPVNEG"                               , "PP"           , TYPE_INT       , 0},
    {"CPVMULT"                              , "PFP"          , TYPE_INT       , 0},
    {"CPVPERP"                              , "PP"           , TYPE_INT       , 0},
    {"CPVRPERP"                             , "PP"           , TYPE_INT       , 0},
    {"CPVNORMALIZE"                         , "PP"           , TYPE_INT       , 0},
    {"CPVNORMALIZE_SAFE"                    , "PP"           , TYPE_INT       , 0},
    {"CPVPROJECT"                           , "PPP"          , TYPE_INT       , 0},
    {"CPVROTATE"                            , "PPP"          , TYPE_INT       , 0},
    {"CPVUNROTATE"                          , "PPP"          , TYPE_INT       , 0},
    {"CPVFORANGLE"                          , "FP"           , TYPE_INT       , 0},
    {"CPVCLAMP"                             , "PFP"          , TYPE_INT       , 0},
    {"CPVLERP"                              , "PPFP"         , TYPE_INT       , 0},
    {"CPVLERPCONST"                         , "PPFP"         , TYPE_INT       , 0},
    {"CPVSLERP"                             , "PPFP"         , TYPE_INT       , 0},
    {"CPVSLERPCONST"                        , "PPFP"         , TYPE_INT       , 0},
    {"CPVNEAR"                              , "PPF"          , TYPE_INT       , 0},
    {"CPVDOT"                               , "PP"           , TYPE_FLOAT     , 0},
    {"CPVCROSS"                             , "PP"           , TYPE_FLOAT     , 0},
    {"CPVLENGTH"                            , "P"            , TYPE_FLOAT     , 0},
    {"CPVLENGTHSQ"                          , "P"            , TYPE_FLOAT     , 0},
    {"CPVTOANGLE"                           , "P"            , TYPE_FLOAT     , 0},
    {"CPVDISTSQ"                            , "PP"           , TYPE_FLOAT     , 0},
    {"CPVDIST"                              , "PP"           , TYPE_FLOAT     , 0},
    {"CPVSTR"                               , "P"            , TYPE_STRING    , 0},
    {"CPV"                                  , "FFP"          , TYPE_POINTER   , 0},
    {"CPBBWRAPVECT"                         , "PPP"          , TYPE_INT       , 0},
    {"CPBBCLAMPVECT"                        , "PPP"          , TYPE_INT       , 0},
    {"CPBBEXPAND"                           , "PPP"          , TYPE_INT       , 0},
    {"CPBBCONTAINSVECT"                     , "PP"           , TYPE_INT       , 0},
    {"CPBBMERGE"                            , "PPP"          , TYPE_INT       , 0},
    {"CPBBCONTAINSBB"                       , "PP"           , TYPE_INT       , 0},
    {"CPBBINTERSECTS"                       , "PP"           , TYPE_INT       , 0},
    {"CPBBNEW"                              , "FFFFP"        , TYPE_INT       , 0},
    {"DEFBODYP"                             , "IIP"          , TYPE_INT       , 0},
    {"DEFBODYF"                             , "IIF"          , TYPE_INT       , 0},
    {"DEFBODYI"                             , "III"          , TYPE_INT       , 0},
    {"GETBODY"                              , ""             , TYPE_INT       , 0},
    {"CPBODYLOCAL2WORLD"                    , "IPP"          , TYPE_INT       , 0},
    {"GETBODY"                              , "II"           , TYPE_FLOAT     , 0},
    {"GETBODY"                              , "IIP"          , TYPE_INT       , 0},
    {"CPMOMENTFORCIRCLE"                    , "FFFP"         , TYPE_FLOAT     , 0},
    {"CPMOMENTFORSEGMENT"                   , "FPP"          , TYPE_FLOAT     , 0},
    {"CPMOMENTFORPOLY"                      , "FIPP"         , TYPE_FLOAT     , 0},
    {"CPMOMENTFORBOX"                       , "FFF"          , TYPE_FLOAT     , 0},
    {"CPMOMENTFORCIRCLE"                    , "FFFFF"        , TYPE_FLOAT     , 0},
    {"CPMOMENTFORSEGMENT"                   , "FFFFF"        , TYPE_FLOAT     , 0},
    {"CPMOMENTFORPOLY"                      , "FIPFF"        , TYPE_FLOAT     , 0},
    {"CPAREAFORCIRCLE"                      , "FF"           , TYPE_FLOAT     , 0},
    {"CPAREAFORSEGMENT"                     , "FFFFF"        , TYPE_FLOAT     , 0},
    {"CPAREAFORPOLY"                        , "IP"           , TYPE_FLOAT     , 0},
    {"CPBODYSETMASS"                        , "IF"           , TYPE_INT       , 0},
    {"CPBODYSETMOMENT"                      , "IF"           , TYPE_INT       , 0},
    {"CPBODYSETANGLE"                       , "IF"           , TYPE_INT       , 0},
    {"CPBODYUPDATEPOSITION"                 , "IF"           , TYPE_INT       , 0},
    {"CPBODYRESETFORCES"                    , "I"            , TYPE_INT       , 0},
    {"CPBODYISSLEEPING"                     , "I"            , TYPE_INT       , 0},
    {"CPBODYSLEEP"                          , "I"            , TYPE_INT       , 0},
    {"CPBODYACTIVATE"                       , "I"            , TYPE_INT       , 0},
    {"CPBODYISSTATIC"                       , "I"            , TYPE_INT       , 0},
    {"CPBODYISROGUE"                        , "I"            , TYPE_INT       , 0},
    {"CPBODYSLEEPWITHGROUP"                 , "II"           , TYPE_INT       , 0},
    {"CPBODYAPPLYFORCE"                     , "IPP"          , TYPE_INT       , 0},
    {"CPBODYAPPLYIMPULSE"                   , "IPP"          , TYPE_INT       , 0},
    {"CPBODYWORLD2LOCAL"                    , "IPP"          , TYPE_INT       , 0},
    {"CPBODYSLEW"                           , "IPF"          , TYPE_INT       , 0},
    {"CPBODYUPDATEVELOCITY"                 , "IPFF"         , TYPE_INT       , 0},
    {"DEFSHAPEI"                            , "III"          , TYPE_INT       , 0},
    {"DEFSHAPEF"                            , "IIF"          , TYPE_INT       , 0},
    {"DEFSHAPEP"                            , "IIP"          , TYPE_INT       , 0},
    {"CPPOLYSHAPEGETVERT"                   , "IIP"          , TYPE_INT       , 0},
    {"CPRECENTERPOLY"                       , "IP"           , TYPE_INT       , 0},
    {"CPCENTROIDFORPOLY"                    , "IPP"          , TYPE_INT       , 0},
    {"CPSEGMENTSHAPEGETRADIUS"              , "I"            , TYPE_FLOAT     , 0},
    {"CPCIRCLESHAPEGETRADIUS"               , "I"            , TYPE_FLOAT     , 0},
    {"CPSEGMENTSHAPEGETNORMAL"              , "IP"           , TYPE_INT       , 0},
    {"CPCIRCLESHAPEGETOFFSET"               , "IP"           , TYPE_INT       , 0},
    {"CPSEGMENTSHAPEGETA"                   , "IP"           , TYPE_INT       , 0},
    {"CPSEGMENTSHAPEGETB"                   , "IP"           , TYPE_INT       , 0},
    {"CPPOLYSHAPEGETNUMVERTS"               , "I"            , TYPE_INT       , 0},
    {"CPSHAPECACHEBB"                       , "IP"           , TYPE_INT       , 0},
    {"CPRESETSHAPEIDCOUNTER"                , ""             , TYPE_INT       , 0},
    {"GETCPSHAPEI"                          , "II"           , TYPE_INT       , 0},
    {"GETCPSHAPEF"                          , "II"           , TYPE_FLOAT     , 0},
    {"GETCPSHAPEP"                          , "IIP"          , TYPE_INT       , 0},
    {"CPSPACEPOINTQUERYFIRST"               , "PII"          , TYPE_INT       , 0},
    {"CPSHAPEPOINTQUERY"                    , "IP"           , TYPE_INT       , 0},
    {"CPSHAPESEGMENTQUERY"                  , "IPPP"         , TYPE_INT       , 0},
    {"CPSEGMENTQUERYHITPOINT"               , "PPPP"         , TYPE_INT       , 0},
    {"CPSEGMENTQUERYHITDIST"                , "PPP"          , TYPE_FLOAT     , 0},
    {"CPSPACESEGMENTQUERYFIRST"             , "PPIIP"        , TYPE_INT       , 0},
    {"SLEW"                                 , "FFF"          , TYPE_INT       , 0},
    {"UPDATEVELOCITY"                       , ""             , TYPE_INT       , 0},
    {"UPDATEVELOCITY"                       , "F"            , TYPE_INT       , 0},
    {"UPDATEVELOCITYAG"                     , "I"            , TYPE_INT       , 0},
    {"LOCAL2WORLD"                          , "IFFPP"        , TYPE_INT       , 0},
    {"WORLD2LOCAL"                          , "IFFPP"        , TYPE_INT       , 0},
    {"APPLYIMPULSE"                         , "IFFFF"        , TYPE_INT       , 0},
    {"RESETFORCES"                          , "I"            , TYPE_INT       , 0},
    {"APPLYFORCE"                           , "IFFFF"        , TYPE_INT       , 0},
    {"SLEEP"                                , "I"            , TYPE_INT       , 0},
    {"ACTIVATE"                             , "I"            , TYPE_INT       , 0},
    {"ISSTATIC"                             , "I"            , TYPE_INT       , 0},
    {"ISROGUE"                              , "I"            , TYPE_INT       , 0},
    {"ISSLEEPING"                           , "I"            , TYPE_INT       , 0},
    {"CALCULATECONVEXBODY"                  , "IIP"          , TYPE_INT       , 0},
    {"INTERSECTS"                           , "II"           , TYPE_INT       , 0},
    {"CONTAINS"                             , "II"           , TYPE_INT       , 0},
    {"CONTAINSVEC"                          , "IFF"          , TYPE_INT       , 0},
    {"ACTIVATEPROCESSTOUCHINGME"            , ""             , TYPE_INT       , 0},
    {"ADDSEGMENTSHAPETO"                    , "FFFFFI"       , TYPE_INT       , 0},
    {"ADDCIRCLESHAPE"                       , "FFF"          , TYPE_INT       , 0},
    {"ADDSEGMENTSHAPE"                      , "FFFFF"        , TYPE_INT       , 0},
    {"ADDPOLYSHAPE"                         , "FFIP"         , TYPE_INT       , 0},
    {"SPACEPOINTQUERYFIRST"                 , "FFII"         , TYPE_INT       , 0},
    {"SPACESEGMENTQUERYFIRST"               , "FFFFIIP"      , TYPE_INT       , 0},
    {"COLLISIONHANDLERNEW"                  , "II"           , TYPE_INT       , 0},
    {"REMOVECOLLISIONHANDLER"               , "I"            , TYPE_INT       , 0},
    {"GETCOLLISIONINFO"                     , "IP"           , TYPE_INT       , 0},
    {"SETPINJOINTPROPERTIES"                , "IIFF"         , TYPE_INT       , 0},
    {"SETPINJOINTPROPERTIES"                , "IIF"          , TYPE_INT       , 0},
    {"SETSLIDEJOINTPROPERTIES"              , "IIFF"         , TYPE_INT       , 0},
    {"SETSLIDEJOINTPROPERTIES"              , "IIF"          , TYPE_INT       , 0},
    {"SETPIVOTJOINTPROPERTIES"              , "IIFF"         , TYPE_INT       , 0},
    {"SETGROOVEJOINTPROPERTIES"             , "IIFF"         , TYPE_INT       , 0},
    {"SETDAMPEDSPRINGPROPERTIES"            , "IIFF"         , TYPE_INT       , 0},
    {"SETDAMPEDSPRINGPROPERTIES"            , "IIF"          , TYPE_INT       , 0},
    {"SETDAMPEDROTARYSPRINGPROPERTIES"      , "IIF"          , TYPE_INT       , 0},
    {"SETROTARYLIMITJOINTPROPERTIES"        , "IIF"          , TYPE_INT       , 0},
    {"SETRATCHETJOINTPROPERTIES"            , "IIF"          , TYPE_INT       , 0},
    {"SETGEARJOINTPROPERTIES"               , "IIF"          , TYPE_INT       , 0},
    {"SETSIMPLEMOTORPROPERTIES"             , "IIF"          , TYPE_INT       , 0},
    {"GETPINJOINTPROPERTIES"                , "IIPP"         , TYPE_INT       , 0},
    {"GETPINJOINTPROPERTIES"                , "II"           , TYPE_FLOAT     , 0},
    {"GETSLIDEJOINTPROPERTIES"              , "IIPP"         , TYPE_INT       , 0},
    {"GETSLIDEJOINTPROPERTIES"              , "II"           , TYPE_FLOAT     , 0},
    {"GETGROOVEJOINTPROPERTIES"             , "IIPP"         , TYPE_INT       , 0},
    {"GETDAMPEDSPRINGPROPERTIES"            , "IIPP"         , TYPE_INT       , 0},
    {"GETDAMPEDSPRINGPROPERTIES"            , "II"           , TYPE_FLOAT     , 0},
    {"GETDAMPEDROTARYSPRINGPROPERTIES"      , "II"           , TYPE_FLOAT     , 0},
    {"GETROTARYLIMITJOINTPROPERTIES"        , "II"           , TYPE_FLOAT     , 0},
    {"GETRATCHETJOINTPROPERTIES"            , "II"           , TYPE_FLOAT     , 0},
    {"GETGEARJOINTPROPERTIES"               , "II"           , TYPE_FLOAT     , 0},
    {"GETSIMPLEMOTORPROPERTIES"             , "II"           , TYPE_FLOAT     , 0},
    {"GETPIVOTJOINTPROPERTIES"              , "IIPP"         , TYPE_INT       , 0},
    {"GETCONSTRAINTS"                       , "I"            , TYPE_INT       , 0},
    {"GETSHAPES"                            , "I"            , TYPE_INT       , 0},
    {"ADDINANIMATESHAPE"                    , "IFFF"         , TYPE_INT       , 0},
    {"ADDINANIMATESHAPE"                    , "IFFFFF"       , TYPE_INT       , 0},
    {"ADDINANIMATESHAPE"                    , "IFFIP"        , TYPE_INT       , 0},
    {"SETENDPOINTSLINE"                     , "FFFF"         , TYPE_INT       , 0},
    {"SETRADIUSLINE"                        , "F"            , TYPE_INT       , 0},
    {"SETOFFSETCIRCLE"                      , "FF"           , TYPE_INT       , 0},
    {"SETRADIUSCIRCLE"                      , "F"            , TYPE_INT       , 0},
    {"SETVERTCONVEXPOLIGON"                 , "FFIP"         , TYPE_INT       , 0},
    {"SETENDPOINTSLINE"                     , "IFFFF"        , TYPE_INT       , 0},
    {"SETRADIUSLINE"                        , "IF"           , TYPE_INT       , 0},
    {"SETOFFSETCIRCLE"                      , "IFF"          , TYPE_INT       , 0},
    {"SETRADIUSCIRCLE"                      , "IF"           , TYPE_INT       , 0},
    {"SETVERTCONVEXPOLIGON"                 , "IFFIP"        , TYPE_INT       , 0},
    {"SHAPECACHEBB"                         , "IP"           , TYPE_INT       , 0},
    {"GETOPTIMALINERTIA"                    , "II"           , TYPE_FLOAT     , 0},
    {"GETOPTIMALINERTIA"                    , "IIFF"         , TYPE_FLOAT     , 0},
    {"SPACEUSESPATIALHASH"                  , "FI"           , TYPE_INT       , 0},
    {"SPACEADDCOLLISIONHANDLER"             , "IISSSSPPPP"   , TYPE_INT       , 0},
    {"SPACEREMOVECOLLISIONHANDLER"          , "II"           , TYPE_INT       , 0},
    {"INTERSECTSBB"                         , "I"            , TYPE_INT       , 0},
    {"GETARBITERCANTCONTACTPOINTS"          , "I"            , TYPE_INT       , 0},
    {"ARBITERISFIRSTCONTACT"                , "I"            , TYPE_INT       , 0},
    {"GETARBITERNORMAL"                     , "IIPP"         , TYPE_INT       , 0},
    {"GETARBITERPOINT"                      , "IIPP"         , TYPE_INT       , 0},
    {"GETARBITERDEPTH"                      , "II"           , TYPE_FLOAT     , 0},
    {"GETARBITERTOTALIMPULSEWITHFRICTION"   , "IPP"          , TYPE_INT       , 0},
    {"GETARBITERTOTALIMPULSE"               , "IPP"          , TYPE_INT       , 0},
    {"SETARBITERSURFACEVELOCITY"            , "IFF"          , TYPE_INT       , 0},
    {"GETARBITERSURFACEVELOCITY"            , "IPP"          , TYPE_INT       , 0},
    {"SETARBITERFRICTION"                   , "IF"           , TYPE_INT       , 0},
    {"GETARBITERFRICTION"                   , "I"            , TYPE_FLOAT     , 0},
    {"SETARBITERELASTICITY"                 , "IF"           , TYPE_INT       , 0},
    {"GETARBITERELASTICITY"                 , "I"            , TYPE_FLOAT     , 0},
    {"MODARBITERPROCESSA"                   , "I"            , TYPE_INT       , 0},
    {"MODARBITERPROCESSB"                   , "I"            , TYPE_INT       , 0},
    {"CPSPACEACTIVATESHAPESTOUCHINGSHAPE"   , "I"            , TYPE_INT       , 0},
    {"SPACERESIZESTATICHASH"                , "FI"           , TYPE_INT       , 0},
    {"SPACERESIZEACTIVEHASH"                , "FI"           , TYPE_INT       , 0},
    {"MODGETSTATICBODY"                     , ""             , TYPE_INT       , 0},
    {"SPACEUSESPATIALHASH"                  , "FI"           , TYPE_INT       , 0},
    {"SPACEGETCURRENTTIMESTEP"              , ""             , TYPE_FLOAT     , 0},
    {"ADDDAMPEDSPRING"                      , "IIFFFFFFF"    , TYPE_INT       , 0},
    {"ADDPIVOTJOINT"                        , "IIFF"         , TYPE_INT       , 0},
    {"ADDDAMPEDROTARYSPRING"                , "IIFFF"        , TYPE_INT       , 0},
    {"ADDROTARYLIMITJOINT"                  , "IIFF"         , TYPE_INT       , 0},
    {"ADDRATCHETJOINT"                      , "IIFF"         , TYPE_INT       , 0},
    {"ADDSIMPLEMOTOR"                       , "IIF"          , TYPE_INT       , 0},
    {"ADDGROOVEJOINT"                       , "IIFFFFFF"     , TYPE_INT       , 0},
    {"ADDSLIDEJOINT"                        , "IIFFFFFF"     , TYPE_INT       , 0},
    {"ADDPIVOTJOINT2"                       , "IIFFFF"       , TYPE_INT       , 0},
    {"ADDPINJOINT"                          , "IIFFFF"       , TYPE_INT       , 0},
    {"ADDGEARJOINT"                         , "IIFF"         , TYPE_INT       , 0},
    {"REMOVECONSTRAINT"                     , "I"            , TYPE_INT       , 0},
    {"CONSTRAINTGETIMPULSE"                 , "I"            , TYPE_FLOAT     , 0},
    {"DEFCONSTRAINTF"                       , "IIF"          , TYPE_INT       , 0},
    {"DEFCONSTRAINTI"                       , "III"          , TYPE_INT       , 0},
    {"GETCONSTRAINTF"                       , "II"           , TYPE_FLOAT     , 0},
    {"GETCONSTRAINTI"                       , "II"           , TYPE_INT       , 0},
    {"CONSTRAINTSETPOSTSOLVEFUNC"           , "IS"           , TYPE_INT       , 0},
    {"CONSTRAINTSETPRESOLVEFUNC"            , "IS"           , TYPE_INT       , 0},
    {"GETCONSTRAINTFATHERA"                 , "I"            , TYPE_INT       , 0},
    {"GETCONSTRAINTFATHERB"                 , "I"            , TYPE_INT       , 0},
    {"GENERATEAPROXPOLY"                    , "IIF"          , TYPE_INT       , 0},
    {"DELETEAPROXPOLY"                      , "I"            , TYPE_INT       , 0},
    {"GETELEMENT"                           , "II"           , TYPE_POINTER   , 0},
    {"CANTELEMENT"                          , "I"            , TYPE_INT       , 0},
    {"METABALL"                             , "IIIPIF"       , TYPE_INT       , 0},
    {"DRAW_WATER"                           , "IIIPIFFIII"   , TYPE_INT       , 0},
    {"SET_EFECTOR"                          , "II"           , TYPE_INT       , 0},
    {"EMULATE_WATER"                        , "P"            , TYPE_INT       , 0},
    {"BBFORCIRCLE"                          , "FFF"          , TYPE_INT       , 0},
    {"ACTIVATESTATIC"                       , "II"           , TYPE_INT       , 0},
    {"SLEEPWITHGROUP"                       , "II"           , TYPE_INT       , 0},
    {0, 0, 0, 0}//TYPE_POINTER
};
#else
extern char __bgdexport( mod_chipmunk, globals_def )[];
extern DLVARFIXUP __bgdexport( mod_chipmunk, globals_fixup )[];
extern char __bgdexport( mod_chipmunk, locals_def )[];
extern DLVARFIXUP __bgdexport( mod_chipmunk, locals_fixup )[];
extern char __bgdexport( mod_chipmunk, types_def )[];
extern DLCONSTANT __bgdexport( mod_chipmunk, constants_def )[];
extern HOOK __bgdexport (mod_chipmunk , handler_hooks ) [];
extern void __bgdexport( mod_chipmunk, module_initialize )();
extern void __bgdexport( mod_chipmunk, module_finalize )();
extern char * __bgdexport( mod_chipmunk, modules_dependency )[];
extern DLSYSFUNCS __bgdexport( mod_chipmunk, functions_exports) [];
#endif

#endif

