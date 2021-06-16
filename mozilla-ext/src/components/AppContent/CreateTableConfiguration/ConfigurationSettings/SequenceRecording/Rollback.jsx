import React from 'react'
import s from '../../CreateTableConfiguration.module.css'

const Rollback = (props) => {

    const rollbacks = props.rbs

    return (
        <div>
            {Array.isArray(rollbacks) ? (
                <div className={s.rollbackContainer}>
                    {rollbacks.map(item => {
                        if (item.to_state === 0) {
                            return <p>Rollback to "Page opened"</p>
                        } else {
                            return <p>Rollback to "Action {item.to_state}"</p>
                        }
                    })}
                </div>
            ) : null}
        </div>
    )
}

export default Rollback
