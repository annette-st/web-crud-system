import React, {useEffect, useState} from 'react'
import s from "../CreateTableConfiguration.module.css"
import {Link} from "react-router-dom";
import Star from '@material-ui/icons/Star';
import StarBorder from '@material-ui/icons/StarBorder'
import FormControlLabel from "@material-ui/core/FormControlLabel";
import Checkbox from "@material-ui/core/Checkbox";

const CreateConfigStep4 = (props) => {

    useEffect(() => {
        props.resetIndexing()
    }, [])

    const filteredHeaders = props.headers.filter((item) => {
        return item.isClicked
    })

    const [indexedHeaders, setIndexedHeaders] = useState(filteredHeaders)

    const saveTableConfig = () => {
        window.parent.postMessage({message: 'update_headers', content: JSON.stringify(indexedHeaders)}, '*')
        window.parent.postMessage({message: 'save_table', content: ''}, '*')
    }

    const handleInputChange = (e) => {
        let id = e.target.id
        let modified = [...indexedHeaders]
        for (let i = 0; i < modified.length; i++) {
            if (+modified[i].id === +id) {
                modified[i].index = !modified[i].index
            }
        }
        setIndexedHeaders(modified)
    }

    return (
        <div className={s.generalStepContainer}>
            <p className={s.pageName}>Mark cells</p>
            <p className={s.stepNumber}>Step 3 of 3</p>
            <div className={s.stepContainer}>
                <p className={s.notes}>Select table headers which will be used for building an index.</p>
                {
                    Array.isArray(filteredHeaders) ? (
                        <div className={s.headersContainer}>
                            {
                                filteredHeaders.map((item) => {
                                    return <FormControlLabel
                                            control={<Checkbox icon={<StarBorder />} checkedIcon={<Star style={{fill: "#1976D2"}} />} name={item.name} />}
                                            label={item.name ? item.name : '[empty header name]'} onChange={handleInputChange}
                                        />
                                })
                            }
                        </div>
                    ) : (
                        <div>no headers</div>
                    )
                }
            </div>
            <div className={s.navButtons}>
                <button onClick={props.prev} className={s.prevButton}>Previous</button>
                <div className={s.configurationSettings}>
                    <Link to={{
                        pathname: '/tableSettings',
                        configName: props.name,
                        configData: '',
                        isRetrieve: false
                    }}
                          onClick={saveTableConfig}
                          className={s.nextButton}
                    >Finish</Link>
                </div>
            </div>
        </div>
    )
}

export default CreateConfigStep4
