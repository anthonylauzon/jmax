/** configfile.h
 * Warning: This file is documented with doxygen.
 * Please ignore any special tags (html...) in this file!
 */ 
/** @file configfile.h
 * Minimalist API to read TINY configuration files.
 * The design principle for this config parser is KISS
 * (keep it simple, stupid). If you need to use bigger
 * configuration files, consider using XML (or .ini, but
 * it is less interesting).
 * Another design choice is that the contents of the
 * configuration file are not mapped into memory,
 * but instead the config parameters are always read
 * from the file, which is opened only when needed.
 *
 * The format of the files is very simple:
 *
 * @code
 * ; comments start with semi-colon
 * sparam = "A string with one backslash: \\"
 * ; Yes, strings use C-style notation
 * fparam = 15.789
 * iparam = -4989
 * boolparam1 = true
 * boolparam2 = false
 * @endcode
 */

/**
 *  The data structure for a config file.
 */
typedef struct
{
    char *filePath;
    /* Space is left here for data to optimize file access
    ** (a cache of file lines' offsets may be a good idea).
    */
} config_file_t;

#ifdef __cplusplus
extern "C" {
#endif

/** Open a configuration access structure by "Open" an existing configuration file.
 *  @param filePath path to the configuration file.
 *  @return A pointer to a config_file_t structure for further access
 *   to the file. Note that the file itself isn't left open after this call.
 *   If the file couldn't be opened, the function returns NULL.
 */
config_file_t *config_file_open(const char *filePath);

/** "Close" an existing configuration file access structure.
 *  The memory used by the structure is freed.
 *  @param pConfigFile The config file structure to be "closed".
 */
void config_file_close(config_file_t *pConfigFile);

/** Read a boolean value from a config file.
 *  @param pConfigFile A config file access structure opened with config_file_open().
 *  @param name The name of a parameter key.
 *  @param pValue A pointer to an integer that will receive the value (0 or 1)
 */
int config_file_get_bool(config_file_t *pConfigFile, const char *name, int *pValue);

/** Read an integer value from a config file.
 *  @param pConfigFile A config file access structure opened with config_file_open().
 *  @param name The name of a parameter key.
 *  @param pValue A pointer to an integer that will receive the value.
 */
int config_file_get_int(config_file_t *pConfigFile, const char *name, int *pValue);

/** Read a string value from a config file.
 *  @param pConfigFile A config file access structure opened with config_file_open().
 *  @param name The name of a parameter key.
 *  @param pValue A pointer to a string buffer that will receive the value.
 *  @param maxLen The maximum length of the pValue string.
 */
int config_file_get_string(config_file_t *pConfigFile, const char *name, char *pValue, int maxLen);

/** Read a float value from a config file.
 *  @param pConfigFile A config file access structure opened with config_file_open().
 *  @param name The name of a parameter key.
 *  @param pValue A pointer to a float that will receive the value.
 */
int config_file_get_float(config_file_t *pConfigFile, const char *name, float *pValue);

#ifdef __cplusplus
}
#endif