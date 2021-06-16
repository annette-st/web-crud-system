import React from 'react'
import Switch from "@material-ui/core/Switch"
import withStyles from "@material-ui/core/styles/withStyles";

const RetrieveSwitch = withStyles({
    switchBase: {
        color: '#fff',
        '&$checked': {
            color: '#3496F7',
        },
        '&$checked + $track': {
            backgroundColor: '#3496F7',
        },
    },
    checked: {},
    track: {},
})(Switch)

export default RetrieveSwitch
