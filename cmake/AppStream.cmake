#=======================================================================================================
# Converts a domain string to a compatible AppStream ID
#
# E.G.: org.7-zip.7zip become org._7_zip._7zip
#
# See https://freedesktop.org/software/appstream/docs/chap-Metadata.html#tag-id-generic
#=======================================================================================================
macro(to_appstream_id INPUT OUTPUT)
    set(temp_ "${INPUT}")
    string(REPLACE "-" "_" temp_ ${temp_})
    string(REPLACE "." ";" temp_ ${temp_})
    foreach(section_ ${temp_})
        string(MAKE_C_IDENTIFIER "${section_}" parsed_section_)
        list(APPEND result_ "${parsed_section_}")
    endforeach()
    string(REPLACE ";" "." result_ "${result_}")
    set("${OUTPUT}" "${result_}")
    unset(temp_)
    unset(result_)
endmacro()
