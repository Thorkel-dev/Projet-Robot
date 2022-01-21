#ifndef  UTIL_H 
#define UTIL_H 

/**
 *   \file util.h
 *   \brief Mes macros pour faire des traces et des assertions
  *  \warning Ne fonctionne qu'avec le compilo gcc pour les fonctions TRACE
  *  \author Jerome Delatour
  *  \date Decembre 2012
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#ifndef NDEBUG
#define TRACE(fmt, ...) do {fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__); fflush (stderr); } while(0);    
#define ASSERT_PRINTERROR(assertion) do {if (!(assertion)) {perror("Erreur"); assert(assertion);} } while(0);    
#define STOP_ON_ERROR(error_condition) do {     \
	if(error_condition) { \
		fprintf (stderr, "*** Error (%s) at \
		%s:%d\nExiting\n", \
		#error_condition, __FILE__, __LINE__); \
		perror (""); \
		_exit (1); } \
    } while (0); 

#define TRACE_PUML_START fprintf(stderr, "%s\n", "@startuml"); fflush(stderr); 
#define TRACE_PUML_END fprintf(stderr, "%s\n", "@enduml"); fflush(stderr); 
#define TRACE_PUML(fmt, ...) fprintf(stderr,fmt, ##__VA_ARGS__); fflush(stderr); 

#else
#define TRACE(fmt, ...) 
#define ASSERT_PRINTERROR(assertion)
#define TRACE_PUML(fmt, ...)
#define TRACE_PUML_START 
#define TRACE_PUML_END

#define STOP_ON_ERROR(error_condition)
#endif 

#endif /* UTIL_H */


