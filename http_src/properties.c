/*
 * properties.c
 *
 * Functions that implement simple property lists.
 *
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "string_util.h"
#include "http_server.h"
#include "properties.h"
#include "varray.h"

/** Definition of an entry in a property list */
typedef struct Property {
	char* name; /** name of property */
	char* val;  /** value of property */
} Property;

/** Definition of a property list */
typedef struct Properties {
	VArray* props;  			/** VArray of properties */
} Properties;

/**
 * Create a new properties.
 * @return a new properties
 */
Properties* newProperties() {
	Properties* props = malloc(sizeof(Properties));
	props->props = newVArray(sizeof(Property), 4);
	return props;
}

/**
 * Delete a properties
 * @param a properties
 */
void deleteProperties(Properties* props) {

	// frees property names and value strings
	size_t nprops = sizeVArray(props->props);
	for (int i = 0; i < nprops; i++) {
		Property* prop = elementAtVArray(props->props, i);
		free(prop->name);
		free(prop->val);
	}

	deleteVArray(props->props);  // frees varray
	props->props = NULL;  // reset varray field

	// frees struct
	free(props);
}

/**
 * Put a property to the properties.
 * @param a properties
 * @param name a property name
 * @param val a property value
 * @return true if property added
 */
bool putProperty(Properties* props, const char* name, const char* val) {
	size_t nprops = nProperties(props);
	Property* prop = elementAtVArray(props->props, nprops);
	if (prop == NULL) {
		return false;
	}
	prop->name = strdup(name);
	prop->val = strdup(val);

	return true;
}

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
bool getProperty(Properties* props, size_t propIndex, char* name, char* val) {
	size_t nprops = nProperties(props);
	if (propIndex >= nprops) {
		return false;
	}

	Property* prop = elementAtVArray(props->props, propIndex);

	// return name property truncated to MAX_PROP_NAME bytes
	strlcpy(name, prop->name, MAX_PROP_NAME);

	// return value property truncated to MAX_PROP_VAL-1 length
	strlcpy(val, prop->val, MAX_PROP_VAL);

	return true;
}

/**
 * Find a property by name, starting with specified property index.
 * Name is trucated to MAX_PROP_NAME-1 characters, and value is
 * truncated to MAX_PROP_VAL-1 charcters. Property comparison is
 * case-independent.
 *
 * @param props the properties
 * @param propIndex the starting property index
 * @param name prop name
 * @param val storage for the value
 * @return the index of the value found or SIZE_MAX if not found
 */
size_t findProperty(Properties* props, size_t propIndex, const char* name, char* val) {
	size_t nprops = nProperties(props);
	while (propIndex < nprops) {
		Property* prop = elementAtVArray(props->props, propIndex);
		if (strcasecmp(name, prop->name) == 0) {
			// return value property truncated to MAX_PROP_VAL-1 length
			strlcpy(val, prop->val, MAX_PROP_VAL);
			return propIndex;
		}
		propIndex++;
	}
	return SIZE_MAX;
}

/**
 * Return number of properties.
 * @param props the properties
 * @return number of properties;
 */
size_t nProperties(const Properties* props) {
	return sizeVArray(props->props);
}

/**
 * Store properties to properties file.
 *
 * @paam propFile the properties file
 * @param props the properties
 * @return true if successful, false if cannot create file
 */
bool storeProperties(const char *propFile, Properties* props) {
	// create properties file
	FILE* propStream = fopen(propFile, "w");
	if (propStream == NULL) {
		return false;
	}

	// output header
	int nprops = nProperties(props);
	fprintf(propStream, "# Properties size=%d\n", nprops);

	// output properties
	for (int i = 0; i < nprops; i++) {
		Property* prop = elementAtVArray(props->props, i);
		fprintf(propStream, "%s=%s\n", prop->name, prop->val);
	}

	// close properties file
	fclose(propStream);

	return true;
}

/**
 * Load properties from properties file.
 *
 * @paam propFile the properties file
 * @param props the properties
 * @return number of properties read
 */
int loadProperties(const char *propFile, Properties* props) {
	FILE* propStream = fopen(propFile, "r");
	if (propStream == NULL) {
		return 0;
	}
	char buf[MAXBUF];
	int nprops = 0;

	// get next line
	while (fgets(buf, MAXBUF, propStream) != NULL) {
		if (buf[0] == '#') { // ignore comment
			continue;
		}
		char *valp = strchr(buf, '=');
		if (valp != NULL) {
			//
			*valp++ = '\0';

			// trim newline characters
			trim_newline(valp);

			// make property entry
			putProperty(props, buf, valp);
			nprops++;
		}
	}
	fclose(propStream);
	return nprops;
}

/**
 * Convert properties to null-terminated array
 * of property name/value strings.
 *
 * @param props the properties
 * @return null-terminated array of property name/value strings;
 *   storage for array and strings is allocated and must be freed
 */
char **toPropertiesArray(Properties* props) {
	size_t nprops = nProperties(props);
	char** propsArray = malloc((nprops+1)*sizeof(char*));
	char key[MAXBUF], value[MAXBUF];
	for (int i = 0; i < nprops; i++) {
		getProperty(props, i, key, value);
		propsArray[i] = malloc(strlen(key) + strlen(value) + 2);
		sprintf(propsArray[i], "%s=%s", key, value);
	}
	propsArray[nprops] = NULL;

	return propsArray;
}

/**
 * Delete a null-terminated array of property
 * name/value string pointers.
 *
 * @param propArray the properties array
 */
void deletePropertiesArray(char* propArray[]) {
	for (int i = 0; propArray[i] != NULL; i++) {
		free(propArray[i]);
		propArray[i] = NULL;
	}
	free(propArray);
}
