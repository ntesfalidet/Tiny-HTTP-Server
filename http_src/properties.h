/*
 * properties.h
 *
 * Functions that implement simple property lists.
 *
 */

#ifndef PROPERTIES_H_
#define PROPERTIES_H_
#include <stdbool.h>
#include <stdint.h>

#define MAX_PROP_NAME 128
#define MAX_PROP_VAL 2048

/** Declaration of Properties as opaque type */
typedef struct Properties Properties;

/**
 * Create a new properties.
 * @return a new properties
 */
Properties* newProperties();

/**
 * Delete a properties
 * @param a properties
 */
void deleteProperties(Properties* props);

/**
 * Put a property to the properties.
 * @param a properties
 * @param name a property name
 * @param val a property value
 * @return true if property added
 */
bool putProperty(Properties* props, const char* name, const char* val);

/**
 * Get name and value for the specified property index.
 * Name is trucated to MAX_PROP_NAME-1 characters, and
 * value is truncated to MAX_PROP_VAL-1 charcters.
 *
 * @param props a properties
 * @param propIndex the property index
 * @param name storage for the name
 * @param val storage for the value
 * @return true if property at specified index is available
 */
bool getProperty(Properties* props, size_t propIndex, char* name, char* val);

/**
 * Find a property by name, starting with specified property index.
 * Name is trucated to MAX_PROP_NAME-1 characters, and value is
 * truncated to MAX_PROP_VAL-1 charcters. Property comparison is
 * case-independent.
 *
 * @param props the properties
 * @param propIndex the starting property index
 * @param name prop name
 * @param val storage the value
 * @return the index of the value found or SIZE_MAX if not found
 */
size_t findProperty(Properties* props, size_t propIndex, const char* name, char* val);

/**
 * Return number of properties.
 * @param props the properties
 * @return number of properties;
 */
size_t nProperties(const Properties* props);

/**
 * Store properties to properties file.
 *
 * @paam propFile the properties file
 * @param props the properties
 * @return true if successful, false if cannot create file
 */
bool storeProperties(const char* propFile, Properties* props);

/**
 * Load properties from properties file.
 *
 * @param propFile the properties file
 * @param props the properties
 * @return number of properties read
 */
int loadProperties(const char* propFile, Properties* props);

/**
 * Convert properties to null-terminated array
 * of property name/value strings.
 *
 * @param props the properties
 * @return null-terminated array of property name/value strings;
 *   storage for array and strings is allocated and must be freed
 */
char* *toPropertiesArray(Properties* props);

/**
 * Delete a null-terminated array of property
 * name/value string pointers.
 *
 * @param propArray the properties array
 */
void deletePropertiesArray(char* propArray[]);

#endif /* PROPERTIES_H_ */
