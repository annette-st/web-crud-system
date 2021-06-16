import React from "react"
import TabContent from "./TabContent"
import s from '../../CreateTableConfiguration.module.css'
import icn from '../../../../../assets/icn-check-mark.svg'

function Tabs(props) {

    const [ active, setActive ] = React.useState(0)

    const operations = [
        { title: 'delete', content: props.items.crud_operations ? props.items.crud_operations.delete : [] },
        { title: 'create', content: props.items.crud_operations ? props.items.crud_operations.create : [] },
        { title: 'update', content: props.items.crud_operations ? props.items.crud_operations.update : [] }
    ]

    const openTab = e => {
        let btn = e.target.closest('button')
        setActive(+btn.dataset.index)
        props.updateCurrentTab(+btn.dataset.index)
    }

    return (
        <div>
            <div className={s.tabLinkWrapper}>
                {operations.map((n, i) => (
                    <button className={s['tabLink' + (i === active ? 'Active' : '')]} onClick={openTab} data-index={i}>
                        "{ n.title[0].toUpperCase() + n.title.slice(1) }"
                        { n.content.length ? ( <img src={icn} className={s.checkSequenceButton} alt="icn-check-mark"/> ) : null }
                    </button>
                ))}
            </div>
            {operations[active] && <TabContent items={props.items} name={props.name} isRetrieve={props.isRetrieve} current={{...operations[active]}} updateConfigData={props.updateConfigData} />}
        </div>
    )
}

export default Tabs
