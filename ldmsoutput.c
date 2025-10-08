/**
* \file ldmsoutput.c
* \brief LDMS Streams Output Implementation
* 
* Functions in this file support output to an LDMS stream. This is currently
* NOT implemented; the prototype code that used to exist is commented out.
*
**/

//
// Skip entire file if not including LDMS support
//
#ifdef INCLUDE_LDMS

static void initializeLDMSOutput()
{
    return;
}

static void finalizeLDMSOutput()
{
    return;
}

/**
* \brief Output hearbeat datapoint LDMS stream (broken!)
*
* This needs completely redone for threaded data. Currently not used.
**/
static void outputLDMSStreamsData()
{
    char str[MAX_JSON_STRLEN]; // DANGEROUS! Use snprintf!
    int i=0, j=0;
    if (appekgStatus != APPEKG_STAT_OK)
        return;
    j += sprintf(&str[j], "%s", "{");
    i += 42;
    /*** TODO: needs major fixing
    for (i = 0; i < metricCount; i++) {
        if (i > 0)
            j += sprintf(&str[j], ", ");
        switch (metrics[i].type) {
        case UINT8:
            j += sprintf(&str[j], " \"%s\":%u, ", metrics[i].name,
                         metrics[i].v.u8val);
            break;
        case UINT16:
            j += sprintf(&str[j], "\"%s\":%u", metrics[i].name,
                         metrics[i].v.u16val);
            break;
        case UINT32:
            j += sprintf(&str[j], "\"%s\":%u", metrics[i].name,
                         metrics[i].v.u32val);
            break;
        case UINT64:
            j += sprintf(&str[j], "\"%s\":%lu", metrics[i].name,
                         metrics[i].v.u64val);
            break;
        case INT8:
            j += sprintf(&str[j], "\"%s\":%d", metrics[i].name,
                         metrics[i].v.i8val);
            break;
        case INT16:
            j += sprintf(&str[j], "\"%s\":%d", metrics[i].name,
                         metrics[i].v.i16val);
            break;
        case INT32:
            j += sprintf(&str[j], "\"%s\":%d", metrics[i].name,
                         metrics[i].v.i32val);
            break;
        case INT64:
            j += sprintf(&str[j], "\"%s\":%ld", metrics[i].name,
                         metrics[i].v.i64val);
            break;
        case FLOAT:
            j += sprintf(&str[j], "\"%s\":%g", metrics[i].name,
                         metrics[i].v.fpval);
            break;
        case DOUBLE:
            j += sprintf(&str[j], "\"%s\":%g", metrics[i].name,
                         metrics[i].v.dpval);
            break;
        case STRING:
            j += sprintf(&str[j], "\"%s\":\"%s\"", metrics[i].name,
                         metrics[i].v.strval);
            break;
        default:
            j += sprintf(&str[j], "\"%s\":\"%s\"", metrics[i].name, "0");
            break;
        }
    }
    j += sprintf(&str[j], "%s", "}");
    ldms_appinst_publish(str);
    ***/
}
#endif // INCLUDE_LDMS_STREAMS


