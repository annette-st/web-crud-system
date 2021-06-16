import React, { useState } from 'react'
import s from './ConfigData.module.css'
import {Link, NavLink} from 'react-router-dom'
import del from '../../../../assets/icn-delete.svg'
import noConfig from '../../../../assets/icn-gears.svg'

const ConfigData = (props) => {

    const [configs, setConfigs] = useState(props.config)

    const setCurrentConfigByName = (name) => {
        window.parent.postMessage({message: 'set_current_config_by_name', content: name}, '*')
    }
    const setCurrentInputConfigByName = (name) => {
        window.parent.postMessage({message: 'set_input_config_by_name', content: name}, '*')
    }

    let deleteConfiguration = (e) => {
        let [type, name] = e.currentTarget.name.split('@')
        console.log(type)
        console.log(name)
        window.parent.postMessage({message: 'delete_configuration', content: {type, name}}, '*')
        props.updateAppState({loading: true})
    }
    const setConfigIsNew = () => {
        window.parent.postMessage({message: 'config_is_new', content: ''}, '*')
    }

    return (
        <div>
            <p className={s.pageName}>Current Page Configuration</p>
            <div>
                {configs ? (
                    <div>
                        {configs.error ? (
                            <div className={s.emptyConfigContainer}>
                                <img src={noConfig} alt="icn-no-config"/>
                                <p className={s.noConfigMessage}>Create config to start working</p>
                            </div>
                        ) : (
                            <div className={s.configContainer}>
                                {configs.table_configurations.length ? (
                                    <div>
                                        {configs.table_configurations
                                            .sort((a, b) => a.name > b.name ? 1 : -1)
                                            .map((item) => {
                                            return <div className={s.configItem}>
                                                <Link to={{
                                                    pathname:'/tableSettings',
                                                    configData: item,
                                                    configName: item.name,
                                                    isRetrieve: item.crud_operations.retrieve
                                                }}
                                                      className={s.configLink}
                                                      onClick={() => setCurrentConfigByName(item.name)}
                                                ><p>{item.name}</p>
                                                </Link>
                                                <button className={s.deleteConfigBtn} name={'table@' + item.name} onClick={deleteConfiguration}>
                                                    <img src={ del } alt="icn-delete"/>
                                                </button>
                                            </div>
                                        })}
                                    </div>
                                ) : null}
                            </div>
                        )}
                    </div>
                ) : null}
                <div className={s.createConfigBtn}>
                    <NavLink onClick={setConfigIsNew} to='/startTableConfig'>New configuration</NavLink>
                </div>
            </div>
        </div>
    )
}

export default ConfigData
