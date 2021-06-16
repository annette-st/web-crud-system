import React from 'react'
import s from './LoadingUserConfig.module.css'
import loading from '../../../../assets/loading.gif'

function LoadingUserConfig(Component) {
    return function LoadingPersonsData({ isLoading, ...props }) {
        if (!isLoading) return <Component {...props} />
        else return (
            <div className={s.emptyConfigContainer}>
                <img src={loading} alt="icn-loading"/>
                <p className="loadingMessage">loading data...</p>
            </div>
        )
    }
}

export default LoadingUserConfig

