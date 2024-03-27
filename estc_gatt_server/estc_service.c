#include "estc_service.h"

#include "app_error.h"
#include "nrf_log.h"

#include "ble.h"
#include "ble_gatts.h"
#include "ble_srv_common.h"

static ret_code_t estc_ble_add_1_characteristics(ble_estc_service_t *service);
static ret_code_t estc_ble_add_notify_characteristics(ble_estc_service_t *service);
static ret_code_t estc_ble_add_identify_characteristics(ble_estc_service_t *service);

ret_code_t estc_ble_service_init(ble_estc_service_t *service)
{
    ret_code_t error_code = NRF_SUCCESS;
    // TODO: 4. Add service UUIDs to the BLE stack table using `sd_ble_uuid_vs_add`
    ble_uuid_t service_uuid;
    ble_uuid128_t base_uuid = { ESTC_BASE_UUID };

    service_uuid.uuid = ESTC_SERVICE_UUID;
    service_uuid.type = BLE_UUID_TYPE_BLE;

    error_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
    APP_ERROR_CHECK(error_code);

    // TODO: 5. Add service to the BLE stack using `sd_ble_gatts_service_add`
    error_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &service_uuid, &service->service_handle);
    APP_ERROR_CHECK(error_code);

    error_code = estc_ble_add_1_characteristics(service);
    APP_ERROR_CHECK(error_code);
    error_code = estc_ble_add_notify_characteristics(service);
    APP_ERROR_CHECK(error_code);
    error_code = estc_ble_add_identify_characteristics(service);
    APP_ERROR_CHECK(error_code);

    return error_code;
}

static ret_code_t estc_ble_add_1_characteristics(ble_estc_service_t *service)
{
    ret_code_t error_code = NRF_SUCCESS;

    // TODO: 6.1. Add custom characteristic UUID using `sd_ble_uuid_vs_add`, same as in step 4
    ble_uuid_t service_uuid;
    ble_uuid128_t base_uuid = { ESTC_BASE_UUID };

    service_uuid.uuid = ESTC_GATT_CHAR_1_UUID;
    service_uuid.type = BLE_UUID_TYPE_BLE;
    
    error_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
    APP_ERROR_CHECK(error_code);

    // TODO: 6.5. Configure Characteristic metadata (enable read and write)
    ble_gatts_char_md_t char_md = { 0 };
    char_md.char_props.read = 1;
    char_md.char_props.write = 1;

    // Configures attribute metadata. For now we only specify that the attribute will be stored in the softdevice
    ble_gatts_attr_md_t attr_md = { 0 };
    attr_md.vloc = BLE_GATTS_VLOC_STACK;

    // TODO: 6.6. Set read/write security levels to our attribute metadata using `BLE_GAP_CONN_SEC_MODE_SET_OPEN`
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    // TODO: 6.2. Configure the characteristic value attribute (set the UUID and metadata)
    ble_gatts_attr_t attr_char_value = { 0 };
    attr_char_value.p_uuid = &service_uuid;
    attr_char_value.p_attr_md = &attr_md;

    // TODO: 6.7. Set characteristic length in number of bytes in attr_char_value structure
    attr_char_value.max_len = sizeof(attr_char_value);

    // TODO: 6.4. Add new characteristic to the service using `sd_ble_gatts_characteristic_add`
    error_code = sd_ble_gatts_characteristic_add(service->service_handle, &char_md, &attr_char_value, &service->char_handle);
    APP_ERROR_CHECK(error_code);

    return error_code;
}

static ret_code_t estc_ble_add_notify_characteristics(ble_estc_service_t *service)
{
    ret_code_t error_code = NRF_SUCCESS;
    ble_uuid_t service_uuid;
    ble_uuid128_t base_uuid = { ESTC_BASE_UUID };
    ble_gatts_char_md_t char_md = { 0 };
    ble_gatts_attr_md_t attr_md = { 0 };
    ble_gatts_attr_t attr_char_value = { 0 };

    service_uuid.uuid = ESTC_NOTIFY_CHAR_UUID;
    service_uuid.type = BLE_UUID_TYPE_BLE;

    error_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
    APP_ERROR_CHECK(error_code);

    char_md.char_props.read = true;
    char_md.char_props.notify = true;

    attr_md.vloc = BLE_GATTS_VLOC_STACK;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_char_value.p_uuid = &service_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len = sizeof(uint8_t);

    error_code = sd_ble_gatts_characteristic_add(service->service_handle, &char_md, &attr_char_value, &service->notify_handle);
    APP_ERROR_CHECK(error_code);

    return error_code;
}

static ret_code_t estc_ble_add_identify_characteristics(ble_estc_service_t *service)
{
    ret_code_t error_code = NRF_SUCCESS;
    ble_uuid_t service_uuid;
    ble_uuid128_t base_uuid = { ESTC_BASE_UUID };
    ble_gatts_char_md_t char_md = { 0 };
    ble_gatts_attr_md_t attr_md = { 0 };
    ble_gatts_attr_t attr_char_value = { 0 };

    service_uuid.uuid = ESTC_INDICATION_CHAR_UUID;
    service_uuid.type = BLE_UUID_TYPE_BLE;

    error_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
    APP_ERROR_CHECK(error_code);

    char_md.char_props.read = true;
    char_md.char_props.indicate = true;

    attr_md.vloc = BLE_GATTS_VLOC_STACK;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_char_value.p_uuid = &service_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len = sizeof(uint8_t);

    error_code = sd_ble_gatts_characteristic_add(service->service_handle, &char_md, &attr_char_value, &service->indication_handle);
    APP_ERROR_CHECK(error_code);

    return error_code;
}

static ret_code_t estc_ble_char_value_update(uint16_t conn_handle, uint16_t value_handle, uint8_t type, uint8_t value, uint16_t len)
{
    ble_gatts_hvx_params_t hvx_params;
    ret_code_t error_code = NRF_SUCCESS;

    if (conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = value_handle;
        hvx_params.type   = type;
        hvx_params.offset = 0;
        hvx_params.p_len  = &len;
        hvx_params.p_data = &value;

        error_code = sd_ble_gatts_hvx(conn_handle, &hvx_params);
    }

    return error_code;
}

ret_code_t estc_ble_char_notify_value_update(ble_estc_service_t *service, uint8_t value)
{
    ret_code_t error_code;

    error_code = estc_ble_char_value_update(service->connection_handle, service->notify_handle.value_handle, BLE_GATT_HVX_NOTIFICATION, value, sizeof(uint8_t));

    return error_code;
}

ret_code_t estc_ble_char_indicate_value_update(ble_estc_service_t *service, uint8_t value)
{
    ret_code_t error_code;

    error_code = estc_ble_char_value_update(service->connection_handle, service->indication_handle.value_handle, BLE_GATT_HVX_INDICATION, value, sizeof(uint8_t));

    return error_code;
}