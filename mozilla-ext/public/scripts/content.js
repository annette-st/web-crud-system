browser.runtime.onMessage.addListener(handleBackgroundRequest)

async function handleBackgroundRequest(message, sender, sendResponse) {
    switch (message) {
        case 'open_sidebar':
            appendIframe('sidebarPlugin')
            break
        case 'close_sidebar':
            removeIframe('sidebarPlugin')
            clearIframeData()
            break
    }
}

/* ReactDOM message receiver */
window.addEventListener('message', (e) => {
    switch (e.data.message) {
        case 'get_url':
            postMessageToAddon('parent_url', proxyUrl, getRootDomain(document.location.hostname))
            break
        case 'help':
            // * help page *
            // deleteAllConfigurations()
            break
        case 'page_configuration':
            page_configuration = e.data.content
            console.log('page configuration', page_configuration)
            break
        case 'config_is_new':
            isConfigNew = true
            break
        case 'set_current_config_by_name':
            setCurrentTableByName(e.data.content)
            break
        case 'set_input_config_by_name':
            setCurrentFormByName(e.data.content)
            break
        case 'config_table_name':
            tableInProcess.name = e.data.content
            break
        case 'config_form_name':
            formInProcess.name = e.data.content
            break
        case 'update_seq_name':
            currentSequence.name = e.data.content
            break
        case 'edit_sequence':
            editSequence(e.data.content)
            break
        case 'save_sequence':
            saveSequenceToConfig(e.data.content)
            break
        case 'delete_configuration':
            deleteConfiguration(e.data.content.name, e.data.content.type)
            break
        case 'delete_sequence':
            deleteSequenceFromConfig(e.data.content)
            postMessageToAddon('update_page_config', tableInProcess)
            break
        case 'current_config_name':
            current_config_name = e.data.content
            break
        case 'save_table':
            tableInProcess.table_config = JSON.parse(JSON.stringify(tableParams))
            tableParams = JSON.parse(JSON.stringify(tableParamsSample))
            removeCssHighlightForClass(tableInProcess.table_config.header_class)
            removeCssHighlightForClass(tableInProcess.table_config.header_ch_class)
            removeCssHighlightForClass(tableInProcess.table_config.cell_class)
            removeCssHighlightForClass(tableInProcess.table_config.cell_ch_class)
            break
        case 'define_table':
            defineTableOnPage()
            break
        case 'define_headers_manually':
            detectHeadersManually('headers')
            break
        case 'define_rows_manually':
            detectHeadersManually('rows')
            break
        case 'highlight_headers':
            addCssHighlightForHeaders()
            break
        case 'cancel_highlight_headers':
            removeCssHighlightForHeaders()
            break
        case 'clear_headers':
            isConfigNew = true
            tableParams.columns_metadata.length = 0
            current_config_name = ''
            formInProcess = JSON.parse(JSON.stringify(formInProcessSample))
            tableInProcess = JSON.parse(JSON.stringify(tableInProcessSample))
            break
        case 'recording':
            addDataToConfig(e.data.content)
            break
        case 'update_headers':
            updateHeadersWithParams()
            break
        case 'toggle_retrieve':
            tableInProcess.crud_operations.retrieve = !e.data.content
            break
        case 'start_recording':
            addDataToConfig(e.data.content)
            break
        case 'stop_recording':
            document.removeEventListener('click', action_listener, true)
            removeCssHighlightForTag('a, button')
            break
        case 'start_rerec':
            clearSequenceData()
            addDataToConfig(e.data.content)
            break
        case 'create_rollback':
            document.removeEventListener('click', action_listener, true)
            document.addEventListener('click', rollback_listener, true)
            break
        case 'save_configuration':
            saveConfiguration(e.data.content)
            break
        case 'form_conf_name':
            formInProcess.name = e.data.content
            break
        case 'add_input_field':
            document.addEventListener('click', input_listener)
            break
        case 'give_input_name':
            setNameForInput()
            break
        case 'add_submit_btn':
            document.addEventListener('click', confirm_listener, true)
            break
        case 'toggle_submit_enter':
            formInProcess.submit_by_enter = !formInProcess.submit_by_enter
            break
        case 'add_new_tables_listener':
            // table_might_change
            document.addEventListener('click', onclickListener)
            break
        case 'remove_new_tables_listener':
            document.removeEventListener('click', onclickListener)
            break
        case 'stop_headers_listener':
            document.removeEventListener('click', header_listener, true)
            break
    }
})

let input_listener = (e) => {
    let elem = e.target.closest('INPUT') || e.target.closest('TEXTAREA')
    console.log('you clicked on:')
    console.log(elem)
    let elem_data = {
        id: elem.id,
        tag: elem.tag,
        name: elem.name,
        title: elem.title,
        class: elem.className,
        placeholder: elem.placeholder,
        custom_name: ''
    }
    console.log('elem_data', elem_data)
    current_input_data = JSON.parse(JSON.stringify(elem_data))
    postMessageToAddon('input_field_name', JSON.stringify(elem_data))
}

let confirm_listener = (e) => {
    let elem = e.target.closest('A') || e.target.closest('BUTTON')
    console.log('selected elem:')
    console.log(elem)
    if (elem) {
        let temp = {
            elem_tag: elem.tagName,
            content: elem.textContent
        }
        formInProcess.submit_btn = {...temp}
        document.removeEventListener('click', confirm_listener, true)
    }
}

let action_listener = (e) => {
    let elem = e.target.closest('A') || e.target.closest('BUTTON') || e.target.tagName === 'SPAN'
    console.log('selected element', elem)

    if (elem) {
        currentSequence.steps.push({
            n: currentSequence['steps'].length + 1,
            elem_tag: elem.tagName,
            content: elem.textContent,
            rb: []
        })
        postMessageToAddon('click_catch', JSON.stringify(currentSequence.steps))
        console.log(currentSequence)
    }
}
let rollback_listener = (e) => {
    e.preventDefault()
    e.stopPropagation()
    document.removeEventListener('click', rollback_listener, true)
    postMessageToAddon('rb_request')
    let promise = new Promise((resolve, reject) => {
        window.addEventListener('message', (e) => {
            if (e.data.message === 'rb_required_state') {
                resolve(e.data.content)
            }
        })
    })
    promise.then((res) => {
        console.log('to_state:')
        let toState = +res
        if (!isFinite(toState)) {
            toState = 0
        }
        console.log(toState)
        let rollback_elem = e.target.closest('A') || e.target.closest('BUTTON') || e.target.tagName === 'SPAN'
        console.log('rollback_elem', rollback_elem)

        currentSequence.rollback.push({
            'to_state': toState,
            'from_state': currentSequence.steps.length,
            'elem_tag': e.target.tagName,
            'content': e.target.textContent
        })
        currentSequence.steps[currentSequence.steps.length - 1].rb.push({
            'to_state': toState
        })
        console.log(currentSequence)
        postMessageToAddon('click_catch', JSON.stringify(currentSequence.steps))
        document.addEventListener('click', action_listener, true)
    })
}

function addDataToConfig(data) {
    switch (data) {
        case 'action':
            addCssHighlightForTag('a, button')
            document.addEventListener('click', action_listener, true)
            break
        case 'rollback':
            console.log('adding rollback...')
            document.addEventListener('click', rollback_listener, false)
            break
    }
}

function detectHeadersAutomatically() {
    let allHeaders = $('table thead th')
    all_headers_objects = $('table thead th')
    for (let h of allHeaders) {
        tableParams.columns_metadata.push(h.title || h.getAttribute('aria-label') || h.textContent)
        tableParams.all_columns.push(h.title || h.getAttribute('aria-label') || h.textContent)
    }
    tableParams.manually = false
        // todo: adapt for empty tables
        if (document.getElementsByTagName('td')[0]) {
            let cell = document.getElementsByTagName('td')[0]
            tableParams.cell_ch_class = cell.classList[0]
        } else {
            let header = document.getElementsByTagName('th')[0]
            tableParams.cell_ch_class = header.classList[0]
        }
    postMessageToAddon('headers', JSON.stringify(tableParams.columns_metadata))
}
let header_listener
function detectHeadersManually(action) {
    switch (action) {
        case 'headers':
            tableParams.columns_metadata.length = 0
            let headers = []
            let isHeadersDone
            header_listener = (e) => {
                let elem = e.target.closest('div')
                while (!isHeadersDone) {
                    elem.style.border = '3px solid #095DB0'
                    isHeadersDone = confirm('Is the full header line highlighted?')
                    if (isHeadersDone) {
                        document.removeEventListener('click', header_listener, true)
                        tableParams.header_class = elem.classList[0].toString()
                        tableParams.header_ch_class = elem.children[0].classList[0].toString()
                        console.log('tableParams', tableParams)
                        for (let i = 0; i < elem.children.length; i++) {
                            if (cleanString(elem.children[i].textContent) && elem.children[i].tagName === 'DIV') {
                                elem.children[i].style.border = '3px solid #3496F7'
                                headers.push(cleanString(elem.children[i].textContent))
                            }
                        }
                        tableParams.columns_metadata = [...headers]
                        postMessageToAddon('manual_headers', JSON.stringify(tableParams.columns_metadata))
                    } else {
                        elem.style.border = ''
                        elem = elem.parentElement
                    }
                }
            }
            document.addEventListener('click', header_listener, true)
            break
        case 'rows':
            let isRowsDone
            let row_listener = (e) => {
                let elem = e.target.closest('div')
                while (!isRowsDone) {
                    elem.style.border = '3px solid #095DB0'
                    isRowsDone = confirm('Is the full line highlighted with lines?')
                    if (isRowsDone) {
                        document.removeEventListener('click', row_listener, true)
                        let rows = document.getElementsByClassName(elem.className)
                        for (let i = 0; i  < rows.length; i++) {
                            rows[i].style.border = '3px solid #095DB0'
                        }
                        for (let i = 0; i < elem.children.length; i++) {
                            if (elem.children[i].tagName === 'DIV') {
                                elem.children[i].style.border = '3px solid #3496F7'
                            }
                        }
                        tableParams.cell_class = elem.classList[0].toString()
                        tableParams.cell_ch_class = elem.children[0].classList[0].toString()
                        console.log('tableParams', tableParams)
                        postMessageToAddon('rows_done')
                    } else {
                        elem.style.border = ''
                        elem = elem.parentElement
                    }
                }
            }
            document.addEventListener('click', row_listener, true)
            break
    }
}

async function saveConfiguration(type) {
    let currentData = await getCurrentConfig()
    console.log('current configuration data', currentData)
    console.log('saving table', tableInProcess)
    let requestBody = {
        domain: getRootDomain(document.location.hostname),
        operation: 'put',
        payload: {
            domain: getRootDomain(document.location.hostname),
            table_configurations: [],
            form_configurations: []
        }
    }
    if ('error' in currentData) {
        updateRequestBodyIfEmptyConfig(currentData, requestBody)
        await makeSaveRequestToProxy(requestBody)
            .then(response => console.log(response))
    } else {
        if (!isConfigNew) {
            putNewConfigToRequestBody(currentData, requestBody, type)
        } else {
            putNewConfigToRequestBodyIfNew(currentData, requestBody, type)
        }
        await makeSaveRequestToProxy(requestBody)
            .then(response => console.log(response))
    }
    tableInProcess = JSON.parse(JSON.stringify(tableInProcessSample))
    formInProcess = JSON.parse(JSON.stringify(formInProcessSample))
    tableParams = JSON.parse(JSON.stringify(tableParamsSample))
    currentSequence = JSON.parse(JSON.stringify(currentSequenceSample))
    current_config_name = ''
    console.log('RESULT', requestBody)
    postMessageToAddon('saved_successfully')
}

/* secondary functions */
function postMessageToAddon(message, content = '', domain = '') {
    document.getElementById('sidebarPlugin').contentWindow.postMessage({message, content, domain}, '*')
}
function cleanString(input) {
    let output = ''
    for (let i = 0; i < input.length; i++) {
        if (input.charCodeAt(i) <= 127) {
            output += input.charAt(i)
        }
    }
    return output
}
function updateRequestBodyIfEmptyConfig(currentData, requestBody) {
    console.log('[configuration is empty]')
    delete currentData['error']
    switch (type) {
        case 'table':
            requestBody.payload.table_configurations.push(JSON.parse(JSON.stringify(tableInProcess)))
            break
        case 'form':
            requestBody.payload.form_configurations.push(JSON.parse(JSON.stringify(formInProcess)))
            break
    }
}
async function makeSaveRequestToProxy(requestBody) {
    console.log('fetching...')
    await fetch(proxyUrl, {
        method: 'post',
        body: JSON.stringify(requestBody)
    })
        .then((res) => console.log(res) )
        .catch(alert)
}

function putNewConfigToRequestBody(currentData, requestBody, type) {
    let currentObject = type === 'table' ? tableInProcess : formInProcess
    let typeParameter = `${type}_configurations`

    for (let conf = 0; conf < currentData.table_configurations.length; conf++) {
        if (currentData[typeParameter][conf].name === currentObject.name) {
            currentData[typeParameter].splice(conf, 1)
            break
        }
    }
    // splice it from currentData var
    for (let conf = 0; conf < currentData.table_configurations.length; conf++) {
        if (currentData[typeParameter][conf].name === current_config_name) {
            currentData[typeParameter].splice(conf, 1)
            break
        }
    }
    // concat currentData and current configuration, send it to the proxy
    requestBody.payload[`${type === 'table' ? 'form' : 'table'}_configurations`] = JSON.parse(JSON.stringify(currentData[`${type === 'table' ? 'form' : 'table'}_configurations`]))
    requestBody.payload[typeParameter] = currentData[`${type}_configurations`].concat(JSON.parse(JSON.stringify(currentObject)))
}

function putNewConfigToRequestBodyIfNew(currentData, requestBody, type) {
    let currentObject = type === 'table' ? tableInProcess : formInProcess
    let typeParameter = `${type}_configurations`
    requestBody.payload[`${type === 'table' ? 'form' : 'table'}_configurations`] = JSON.parse(JSON.stringify(currentData[`${type === 'table' ? 'form' : 'table'}_configurations`]))
    requestBody.payload[typeParameter] = currentData[typeParameter].concat(JSON.parse(JSON.stringify(currentObject)))
}

function defineTableOnPage() {
    tableParams.manually = true
    postMessageToAddon('headers')
    const MAX_WAITING_TIME = 5000
    let timePassed = 0
    let int = setInterval(() => {
        let tables = document.getElementsByTagName('thead')
        for (let table of tables) {
            if (table.offsetWidth > 0) {
                clearInterval(int)
                detectHeadersAutomatically()
                break
            }
        }
        if (timePassed >= MAX_WAITING_TIME) {
            clearInterval(int)
            tableParams.manually = true
            postMessageToAddon('headers')
        }
        timePassed += 200
    }, 200)
}

async function deleteAllConfigurations() {
    await fetch(proxyUrl, {
        method: 'post',
        body: JSON.stringify({
            'domain': getRootDomain(document.location.hostname),
            'operation': 'delete'
        })
    }).then((res) => {
        console.log(res)
    })
}

function updateHeadersWithParams() {
    let selectedHeaders = JSON.parse(e.data.content).map((item) => {
        return {
            id: item.id,
            index: item.index,
            name: item.name
        }
    })
    tableParams.columns_metadata = JSON.parse(JSON.stringify(selectedHeaders))
}

function clearSequenceData() {
    currentSequence.steps.length = 0
    currentSequence.rollback.length = 0
}

function setNameForInput() {
    document.removeEventListener('click', input_listener)
    current_input_data.custom_name = e.data.content
    formInProcess.fields.push({...current_input_data})
    current_input_data = JSON.parse(JSON.stringify(input_data_example))
}

let onclickListener = (e) => {
    postMessageToAddon('table_might_change')
}

function setCurrentTableByName(name) {
    console.log('setting config by name...')
    current_config_name = name
    for (let conf of page_configuration.table_configurations) {
        if (conf.name === name) {
            console.log(`config is set: ${conf.name}`)
            tableInProcess = JSON.parse(JSON.stringify(conf))
            isConfigNew = false
            break
        }
    }
}

function setCurrentFormByName(name) {
    console.log('setting config by name...')
    for (let conf of page_configuration.form_configurations) {
        if (conf.name === name) {
            console.log(`config is set: ${conf.name}`)
            formInProcess = JSON.parse(JSON.stringify(conf))
            isConfigNew = false
            break
        }
    }
}

function saveSequenceToConfig(crud) {
    tableInProcess.crud_operations[crud].push(JSON.parse(JSON.stringify(currentSequence)))
    postMessageToAddon('update_page_config', tableInProcess)
    currentSequence.name = ''
    clearSequenceData()
}

function deleteSequenceFromConfig(seqData) {
    for (let seq = 0; seq < tableInProcess.crud_operations[seqData.type].length; seq++) {
        if (tableInProcess.crud_operations[seqData.type][seq].name === seqData.name) {
            tableInProcess.crud_operations[seqData.type].splice(seq, 1)
            break
        }
    }
}

function editSequence(seqData) {
    console.log('edit sequence in process')
    currentSequence.name = ''
    clearSequenceData()
    let oper = seqData.operation
    let seq_name = seqData.name

    for (let i = 0; i < tableInProcess.crud_operations[oper].length; i++) {
        if (tableInProcess.crud_operations[oper][i].name === seq_name) {
            currentSequence = {...tableInProcess.crud_operations[oper][i]}
            tableInProcess.crud_operations[oper].splice(i, 1)
            break
        }
    }
}

/* edit configs functions */
function getRootDomain(hostname) {
    const rootDomainRE = /([\w\d]*\.\w{2,4}$)/
    let rootDomain = hostname.match(rootDomainRE)
    return rootDomain ? rootDomain[1] : null
}

async function deleteConfiguration(name, type) {
    let currentData = await getCurrentConfig()
    let reqConfig = {
        domain: getRootDomain(document.location.hostname),
        operation: 'put',
        // object that injects in corresponding pages
        payload: {
            domain: getRootDomain(document.location.hostname),
            table_configurations: [],
            form_configurations: []
        }
    }
    switch (type) {
        case 'table':
            for (let i = 0; i < currentData.table_configurations.length; i++) {
                if (currentData.table_configurations[i].name === name) {
                    currentData.table_configurations.splice(i, 1)
                    break
                }
            }
            break
        case 'form':
            for (let i = 0; i < currentData.form_configurations.length; i++) {
                if (currentData.form_configurations[i].name === name) {
                    currentData.form_configurations.splice(i, 1)
                    break
                }
            }
            break
    }
    reqConfig.payload.table_configurations = JSON.parse(JSON.stringify(currentData.table_configurations))
    reqConfig.payload.form_configurations = JSON.parse(JSON.stringify(currentData.form_configurations))

    if (!currentData.table_configurations.length && !currentData.form_configurations.length) {
        console.log('configuration is empty...')
        reqConfig = {
            error: 'not found'
        }
    }
    await fetch(proxyUrl, {
        method: 'post',
        body: JSON.stringify(reqConfig)
    })
    console.log('deleted successfully. result:')
    console.log(reqConfig)
    postMessageToAddon('deleted_successfully', '', getRootDomain(document.location.hostname))
    console.log('done')
}

async function getCurrentConfig() {
    return await new Promise((resolve, reject) => {
        fetch(proxyUrl, {
            method: 'post',
            body: JSON.stringify({
                'operation': 'get',
                'domain': getRootDomain(document.location.hostname)
            })
        })
            .then((data) => resolve(data.json()))
    })
}

/* style change functions */
function addCssHighlightForTag(elems) {
    let els = document.querySelectorAll(elems)
    for (let el of els) {
        $(el).hover(
            function () {
                el.style.border = '2px solid #3496F7'
            },
            function () {
                el.style.border = ''
            }
        )
    }
}

function addCssHighlightForHeaders() {
    all_headers_objects = $('table thead th').find('a')
    if (all_headers_objects.length) {
        for (let i = 0; i < all_headers_objects.length; i++) {
            $(all_headers_objects[i]).css({
                border: '2px solid #3496F7'
            })
        }
    }
}

function removeCssHighlightForTag(elems) {
    let els = document.querySelectorAll(elems)
    for (let el of els) {
        $(el).hover(
            function () {
                el.style.border = ''
            },
            function () {
                el.style.border = ''
            }
        )
    }
}

function removeCssHighlightForClass(className) {
    let els = document.getElementsByClassName(className)
    for (let i = 0; i < els.length; i++) {
        els[i].style.border = ''
    }
}

function removeCssHighlightForHeaders() {
    if (all_headers_objects.length) {
        for (let i = 0; i < all_headers_objects.length; i++) {
            $(all_headers_objects[i]).css({
                'border': ''
            })
        }
    }
}

/* addon functions */
function appendIframe(name) {
    let iframe = document.createElement('iframe')
    iframe.src = browser.runtime.getURL('index.html')
    iframe.id = name
    iframe.name = document.location.protocol + '//' + document.location.hostname
    iframe.className = 'scroll'
    iframe.style.cssText = 'position:fixed;right:0;top:102px;display:block;z-index:999999999999999;' +
        'width:300px;height:565px;background:white;border:none;box-shadow: 0px 4px 12px rgba(0, 0, 0, 0.25);'
    document.body.appendChild(iframe)

    addDragnDrop(iframe)
}

function addDragnDrop(elem) {
    let pageMouseX, pageMouseY
    let frameTop = 102;
    let frameLeft = 0;
    elem.style.top = frameTop + 'px'
    elem.style.left = frameLeft + 'px'

    window.addEventListener('message', evt => {
        const data = evt.data

        switch (data.msg) {
            case 'IFRAME_DRAG_START':
                handleDragStart(data.mouseX, data.mouseY)
                break
            case 'IFRAME_DRAG_MOUSEMOVE':
                handleFrameMousemove(data.offsetX, data.offsetY)
                break
            case 'IFRAME_DRAG_END':
                handleDragEnd()
                break
        }
    })

    function handleDragStart (mouseX, mouseY) {
        pageMouseX = frameLeft + mouseX
        pageMouseY = frameTop + mouseY
        document.addEventListener('mouseup', handleDragEnd)
        document.addEventListener('mousemove', handlePageMousemove)
    }

    function handleDragEnd () {
        document.removeEventListener('mouseup', handleDragEnd)
        document.removeEventListener('mousemove', handlePageMousemove)
    }

    function handleFrameMousemove (offsetX, offsetY) {
        frameTop += offsetY
        frameLeft += offsetX
        elem.style.top = frameTop + 'px'
        elem.style.left = frameLeft + 'px'
        pageMouseX += offsetX
        pageMouseY += offsetY
    }

    function handlePageMousemove (evt) {
        frameTop += evt.clientX - pageMouseX
        frameLeft += evt.clientY - pageMouseY
        elem.style.top = frameTop + 'px'
        elem.style.left = frameLeft + 'px'
        pageMouseX = evt.clientX
        pageMouseY = evt.clientY
    }
}

function removeIframe(name) {
    let iframe = document.getElementById(name)
    iframe.parentNode.removeChild(iframe);
}
function clearIframeData() {
    if (all_headers_objects) {
        if (all_headers_objects.length) {
            for (let i = 0; i < all_headers_objects.length; i++) {
                $(all_headers_objects[i]).css({
                    'border': ''
                })
            }
        }
        all_headers_objects.length = 0
    }
}

/* configuration objects */
let page_configuration
let current_input_data
let all_headers_objects
let current_config_name
let isConfigNew = true

let pc = {
    domain: getRootDomain(document.location.hostname),
    table_configurations: [],
    form_configurations: []
}

let formInProcess = {
    name: '',
    type: 'form',
    fields: [],
    submit_btn: {},
    submit_by_enter: false
}

let formInProcessSample = {
    name: '',
    type: 'form',
    fields: [],
    submit_btn: {},
    submit_by_enter: false
}

let input_data_example = {
    id: '',
    tag: '',
    name: '',
    title: '',
    class: '',
    placeholder: '',
    custom_name: ''
}

let tableInProcess = {
    name: '',
    type: 'table',
    //domain: getRootDomain(document.location.hostname),
    crud_operations: {
        'create': [],
        'retrieve': false,
        'update': [],
        'delete': []
    },
    table_config: {}
}

let tableInProcessSample = {
    name: '',
    type: 'table',
    domain: getRootDomain(document.location.hostname),
    crud_operations: {
        'create': [],
        'retrieve': false,
        'update': [],
        'delete': []
    },
    table_config: {}
}

let currentSequence = {
    name: '',
    rollback: [],
    steps: []
}
let currentSequenceSample = {
    name: '',
    rollback: [],
    steps: []
}

let tableParamsSample = {
    columns_metadata: [],
    all_columns: [],
    manually: false
}

let tableParams = {
    columns_metadata: [],
    all_columns: [],
    manually: false
}
let proxyUrl = document.location.protocol + '//' + document.location.hostname + '/3f785034-d25a-11ea-87d0-0242ac130003/store'
