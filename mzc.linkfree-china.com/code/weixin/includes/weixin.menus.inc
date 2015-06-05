<?php

function weixin_menus_configuration_form_submit($form, &$form_state) {
	$menu_type = array('0' => 'click', '1'=> 'view');
//	dpm($form_state);
	$buttons = array();
	foreach((array)$form_state['values']['menu_items'] as $item) {
		$button['type'] = $menu_type[$item['menu_type']];
		$button['name'] = urlencode($item['name']);
		if ($item['menu_type'] == 0) {
			$button['key'] = urlencode($item['key_url']);
		} elseif ($item['menu_type'] == 1) {
			$button['url'] = urlencode($item['key_url']);
		}
		if ($item['weight'] != 5)
		{
			$buttons['button'][10 + $item['weight']] = $button;
		} else {
			$buttons['button'][] = $button;
		}
		unset($button);
	}
	ksort($buttons['button']);
//	dpm($buttons);
	weixin_post_custom_menu($buttons);
}

function weixin_post_custom_menu($buttons) {
	$weixin_access_token = weixin_robot_get_access_token();
	$url = 'https://api.weixin.qq.com/cgi-bin/menu/create?access_token='.$weixin_access_token;
	$options = array (
		'method' => 'POST',
		'data' => urldecode(json_encode($buttons)),
//		'headers' => array('Content-Type' => 'application/x-www-form-urlencoded'),
		'timeout' => 15,
	);
	$request_val = drupal_http_request($url, $options);
//	dpm($request_val);
}

function weixin_menus_configuration_form($form_state) {
	$menu_type = array('0' => 'click', '1'=> 'view');
	$menu_itmes_obj = get_weixin_menu_itmes();
//	dpm($menu_itmes_obj);
	$i = 1;
	$j = 1;
	$form['menu_items']['#tree'] = TRUE;
	if(isset($menu_itmes_obj['menu']['button'])) {
	foreach ((array)$menu_itmes_obj['menu']['button'] as $key => $item) {
//		dpm($key);
		$form['menu_items'][$i] = array(
		  'name' => array(
        		'#type' => 'textfield',
        		'#default_value' => $item['name'],
        		'#size' => 20,
        		'#maxlength' => 255,
		   ),
		  'key_url' => array(
        		'#type' => 'textfield',
        		'#default_value' => isset($item['key'])?$item['key']:$item['url'],
        		'#size' => 20,
        		'#maxlength' => 255,
	      	  ),
		  'menu_type' => array(
			'#type'  => 'select',
			'#multiple' => FALSE,
			'#options' => $menu_type,
        		'#default_value' => isset($item['key'])?0:1,
			'#size' => 1,
		  ),
		  'id' => array(
        		'#type' => 'textfield',
        		'#size' => 3,
        		'#default_value' => $key,
        		'#disabled' => TRUE,
		  ),
		  'pid' => array(
        		'#type' => 'textfield',
        		'#size' => 3,
        		'#default_value' => 0,
      		  ),
		  'weight' => array(
			'#type' => 'weight',
			'#title' => t('Weight'),
			'#default_value' => 5,
			'#delta' => 10,
			'#title_display' => 'invisible',
		  ),
		  'depth' => array(
			'#type' => 'hidden',
			'#value' => 0,
		  ),
		);
		$i++;
		if(!empty($item['sub_button'])) {
				foreach((array)$item['sub_button'] as $sub_item) {
	//			dpm($sub_item);
				$form['menu_items'][$j] = array(
				'name' => array(
					'#type' => 'textfield',
					'#default_value' => $sub_item['name'],
					'#size' => 20,
					'#maxlength' => 255,
				),
				 'key_url' => array(
					'#type' => 'textfield',
					'#default_value' => $sub_item['name'],
					'#size' => 20,
					'#maxlength' => 255,
				),
				  'menu_type' => array(
					'#type'  => 'select',
					'#multiple' => FALSE,
					'#options' => $menu_type,
					'#default_value' => isset($item['key'])?0:1,
					'#size' => 1,
				),
				'id' => array(
					'#type' => 'textfield',
					'#size' => 3,
					'#default_value' => $key,
					'#disabled' => TRUE,
				),
				'pid' => array(
					'#type' => 'textfield',
					'#size' => 3,
					'#default_value' => $j,
				),
				'weight' => array(
					'#type' => 'weight',
					'#title' => t('Weight'),
					'#default_value' => 5,
					'#delta' => 3,
					'#title_display' => 'invisible',
				),
				'depth' => array(
					'#type' => 'hidden',
					'#value' => 1,
				),
				);
				$j++;
			}
		}
		$i++;
	} 
	}
	$form['submit'] = array('#type' => 'submit', '#value' => t('Save Changes'));
	return $form;
}

function theme_weixin_menus_configuration_form($variables) {
	global $base_url;
	$form = $variables['form'];
	$rows = array();
	foreach ((array)element_children($form['menu_items']) as $id) {
//		dpm($id);
		$form['menu_items'][$id]['weight']['#attributes']['class'] = array('menu-item-weight');
    		$form['menu_items'][$id]['id']['#attributes']['class'] = array('menu-item-id');
		$form['menu_items'][$id]['pid']['#attributes']['class'] = array('menu-item-pid');
    		$class = array('draggable');
		/*
		if ($id == '8') {
			$class[] = 'tabledrag-root';
		}
		if ($id == '9') {
			$class[] = 'tabledrag-leaf';
		}
		*/
		$delete_path = $base_url.'/'.'weixin/menus?id='.$form['menu_items'][$id]['id']['#value'].'&op=delete';
		$delete_path_link = '<a href='.$delete_path.'>delete</a>';
		$indent = theme('indentation', array('size' => $form['menu_items'][$id]['depth']['#value']));
		unset($form['menu_items'][$id]['depth']);
		$rows[] = array(
		  'data' => array(
		    $indent . drupal_render($form['menu_items'][$id]['name']),
		    drupal_render($form['menu_items'][$id]['menu_type']),
		    drupal_render($form['menu_items'][$id]['key_url']),
		    drupal_render($form['menu_items'][$id]['weight']),
		    drupal_render($form['menu_items'][$id]['id']),
		    drupal_render($form['menu_items'][$id]['pid']),
		    $delete_path_link,
		  ),
		  'class' => $class,
		);
 	}

	$header = array(t('Name'), t('type'), t('key/url'), t('Weight'), t('ID'), t('PID'),t('OP'));
	$table_id = 'menu-items-table';
	$output = theme('table', array(
		'header' => $header,
		'rows' => $rows,
		'attributes' => array('id' => $table_id),
	));
//	dpm($form);
	/*
	$output .= drupal_render_children($form);
	$output .= drupal_render_children($form);
	*/
	$output .= drupal_render_children($form);

	drupal_add_tabledrag($table_id, 'match', 'parent', 'menu-item-pid', 'menu-item-pid', 'menu-item-id', FALSE);
	drupal_add_tabledrag($table_id, 'order', 'sibling', 'menu-item-weight', NULL, NULL, FALSE);
	return $output;
}

function weixin_menus_configuration() {
	$weixin_menu_item_obj = variable_get('custom_menus');
//	dpm($weixin_menu_item_obj);

	global $base_url;
	$menu_path = $base_url.'/'.'weixin/menus';
	$menu_id = isset($_REQUEST['id'])?$_REQUEST['id']:'';
//	dpm($menu_id);
	$menu_op = isset($_REQUEST['op'])?$_REQUEST['op']:'';
// 	if (! empty($menu_id) && $menu_op == "delete" ) {
 	if ($menu_op == "delete") {
		$weixin_menu_item_obj = variable_get('custom_menus');
//		dpm($weixin_menu_item_obj);
		unset($weixin_menu_item_obj['menu']['button'][$menu_id]);
		variable_set('custom_menus',$weixin_menu_item_obj);
		drupal_goto($menu_path);
	}
	$output ='';
	$output .= drupal_render(drupal_get_form('weixin_menus_configuration_form'));
	$output .= drupal_render(drupal_get_form('weixin_menu_add_form'));
	return $output;	
}

function get_weixin_menu_itmes() {
	$weixin_menu_item_obj = variable_get('custom_menus');
	if (!isset($weixin_menu_item_obj)) {
        	$weixin_robot_access_token = weixin_robot_get_access_token();
		$url = 'https://api.weixin.qq.com/cgi-bin/menu/get?access_token='.$weixin_robot_access_token;
		$request_val = drupal_http_request($url);
		if ($request_val->code != 200) {
			drupal_set_message(t('HTTP error @errorcode occurred when trying to get token.', array('@errorcode' => $request_val->code)), 'error');
    			return FALSE;
		}
		$weixin_menu_item_obj = json_decode($request_val->data,true);
		variable_set('custom_menus',$weixin_menu_item_obj);
	}
//	dpm($weixin_menu_item_obj);
	return $weixin_menu_item_obj;
}

function weixin_menu_add_form($form,&$form_state) {
	$menu_type = array('0' => 'click', '1'=> 'view');
	$form = array();
	$form['description'] = array(
                '#type' => 'item',
                '#title' => t('Add new weixin menu'),
        );
	$form['name'] = array(
		'#title' => t('weixin menu name'),
		'#type' => 'textfield',
//		'#default_value' => $sub_item['name'],
		'#size' => 40,
		'#maxlength' => 255,
	);
	$form['menu_type'] = array(
		'#title' => t('weixin menu type'),
		'#type'  => 'select',
		'#multiple' => FALSE,
		'#options' => $menu_type,
		'#size' => 1,
	);
	$form['key_url'] = array(
		'#title' => t('weixin menu key/url'),
		'#type' => 'textfield',
//		'#default_value' => $sub_item['name'],
		'#size' => 40,
		'#maxlength' => 255,
	);
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => 'Add new menu',
	);
	return $form;
}

function weixin_menu_add_form_submit($form,&$form_state)
{
	$weixin_menu_item_obj = variable_get('custom_menus');
	if (isset($weixin_menu_item_obj)) {
		$button['name'] = $form_state['values']['name'];
		if ($form_state['values']['menu_type'] == 0) {
			$button['type'] = 'click';
			$button['key'] = $form_state['values']['key_url'];
		} else {
			$button['type'] = 'view';
			$button['url'] = $form_state['values']['key_url'];
		}
		$button['sub_button'] = array();
		$weixin_menu_item_obj['menu']['button'][] = $button;
		variable_set('custom_menus',$weixin_menu_item_obj);
	}
}
