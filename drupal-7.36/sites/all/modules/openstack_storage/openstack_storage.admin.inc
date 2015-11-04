<?php
/**
 * @file
 * The admin section for the openstack storage module.
 */

/**
 * Callback form for path 'admin/config/services/openstack_storage/account'.
 */
function openstack_storage_account_settings($form, &$form_state) {
  $form['account'] = array(
    '#type' => 'fieldset',
    '#title' => t('Account Information'),
    '#description' => t('Configure Drupal to connect to your OpenStack account.'),
    '#collapsed' => FALSE,
    '#collapsible' => TRUE,
  );

  $form['account']['openstack_storage_auth_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Endpoint URL'),
    '#description' => t('The endpoint URL used for authentication.'),
    '#size' => 60,
    '#maxlength' => 128,
    '#default_value' => variable_get('openstack_storage_auth_url', ''),
    '#required' => TRUE,
  );

  $form['account']['openstack_storage_tenant_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Tenant ID'),
    '#description' => t('The alphanumeric Tenant ID number assigned by OpenStack.'),
    '#size' => 60,
    '#maxlength' => 128,
    '#default_value' => variable_get('openstack_storage_tenant_id', ''),
    '#required' => TRUE,
  );

  $form['account']['openstack_storage_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#description' => t('The account username.'),
    '#size' => 60,
    '#maxlength' => 128,
    '#default_value' => variable_get('openstack_storage_username', ''),
    '#required' => TRUE,
  );
  $form['account']['openstack_storage_password'] = array(
    '#type' => 'password',
    '#title' => t('Password'),
    '#description' => t('The account password for the given username.'),
    '#size' => 60,
    '#maxlength' => 128,
    '#default_value' => variable_get('openstack_storage_password', ''),
    '#required' => TRUE,
  );

  $client = _openstack_storage_get_client();
  $form['openstack'] = array(
    '#type' => 'fieldset',
    '#title' => t('OpenStack Settings'),
    '#description' => t('Configure parts of OpenStack. This only becomes available after credentials have been entered.'),
    '#collapsed' => FALSE,
    '#collapsible' => TRUE,
  );
  $form['openstack']['openstack_storage_object_store_type'] = array(
    '#type' => 'select',
    '#title' => t('OpenStack Object Store type'),
    '#description' => t('For example "cloud" or "swift".'),
    '#disabled' => !$client,
    '#options' => $client ? _openstack_storage_get_object_store_types($client) : array(),
    '#default_value' => _openstack_storage_get_object_store_type($client),
  );

  $form['openstack']['openstack_storage_object_store_region'] = array(
    '#type' => 'select',
    '#title' => t('OpenStack Object Store Region'),
    '#description' => t('For example "NL" or "USA".'),
    '#disabled' => !$client,
    '#options' => $client ? _openstack_storage_get_catalog_type_regions($client, 'object-store', _openstack_storage_get_object_store_type($client)) : array(),
    '#default_value' => _openstack_storage_get_object_store_region($client),
  );

  return system_settings_form($form);
}

/**
 * Callback form for path 'admin/config/services/openstack_storage/object-store'.
 */
function openstack_storage_object_store_settings($form, &$form_state) {
  $form = array();
  $scheme_map = variable_get('openstack_storage_enabled_schemes', array());

  $store = _openstack_storage_get_object_store();
  $containers = $store->listContainers();

  $options = array('_none' => '-- No Container --');
  $container_names = array();
  foreach ($containers as $container) {
    $label = $container->getName();
    if ($container->isCdnEnabled()) {
      $label .= ' (CDN)';
    }
    $options[$container->getName()] = $label;
    $container_names[] = $container->getName();
  }

  $default_compatibility = t('unknown');
  $compatibility = array(
    'public' => 'compatible',
    'private' => 'compatible',
    'temporary' => 'compatible, but not recommended',
    'openstack' => 'compatible',
  );

  $wrappers = module_invoke_all('stream_wrappers');

  $form['containers'] = array(
    '#type' => 'value',
    '#value' => $container_names,
  );

  foreach ($wrappers as $name => $info) {
    $note = isset($compatibility[$name]) ? $compatibility[$name] : $default_compatibility;
    $default = variable_get('openstack_storage_stream_' . $name, '_none');
    if (!empty($scheme_map[$name])) {
      $default = $scheme_map[$name];
    }

    $form['wrapper_' . $name] = array(
      '#type' => 'fieldset',
      '#title' => $info['name'],
      '#description' => $info['description'],
      '#collapsed' => FALSE,
      '#collapsible' => FALSE,
    );

    $form['wrapper_' . $name]['compatibility'] = array(
      '#markup' => '<p><b>' . t('Compatibility:') . ' </b>' . $note . '</p>',
    );

    $form['wrapper_' . $name]['openstack_storage_stream_' . $name] = array(
      '#type' => 'select',
      '#title' => t('Map to Container'),
      '#description' => t('Link this stream wrapper to a container on your OpenStack object storage. This will store files in the object storage.'),
      '#options' => $options,
      '#default_value' => $default,
    );
  }

  return system_settings_form($form);
}
