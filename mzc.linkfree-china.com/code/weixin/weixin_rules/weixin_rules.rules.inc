<?php

function weixin_rules_rules_event_info() {
	$event = array(
	'weixin_bind_event' => array(
		'label'  => t('Bind wechat id'),
		'group' => t('weixin event'),
		'parameter' => array(
			'recipient' => array(
			'type' => 'user',
			'label' => t('Recipient user'),
			),
			'template_id' => array(
			'type' => 'text',
			'label' => t('template_id'),
			),
			'template_type' => array(
			'type' => 'text',
			'label' => t('template list'),
			'options list' => 'get_weixin_template_type',
			),
		),
		),
	);
	return $event;
}

function weixin_bind_event($user) {
//	dpm($user);
	drupal_set_message('weixin_bind_event');
}

function weixin_rules_rules_action_info() {
  $fields['subject']['description'] = t('Subject of the message. To use the tokens provided by Privatemsg, use {token}, for example: {privatemsg_message:recipient:name}.');
  $actions = array(
    'weixin_user' => array(
      'label' => t('Send weixin to a user'),
      'parameter' => array(
        'recipient' => array(
		'type' => 'user',
		'label' => t('Recipient user'),
	),
        'template_id' => array(
		'type' => 'text',
		'label' => t('template_id'),
	),
        'template_type' => array(
		'type' => 'text',
		'label' => t('template list'),
          	'options list' => 'get_weixin_template_type',
	),
      ),
      'group' => t('Weixin'),
       /*
      'base' => 'rules_action_mail_user',
      'callbacks' => array(
        'validate' => 'rules_action_custom_validation',
        'help' => 'rules_mail_help',
      ),
      */
    ),
  );
  return $actions;
}

function get_weixin_template_type() {
  return array(
    'login' => t('login template'),
    'approved' => t('Automatically approved'),
    'moderated' => t('Added to moderation'),
  );
}

function weixin_rules_replace_tokens($text) {
  return preg_replace('/{([a-z0-9_:]+)}/', '[$1]', $text);
}


function weixin_user($user, $template_id, $template_type, $data, $color='#FF0000') {
	dpm($template_type);
	$template=array();
	$query = db_select('weixin_user_map','user_map');
	$query->condition('uid',$user->uid);
	$query->fields('user_map',array('wx_openid'));
	$result = $query->execute()->fetchAssoc();
	if(!empty($result)) {
		$user_id = $user->uid;
		$template['touser'] = $result['wx_openid'];
		$template['template_id'] = $template_id;
		$template['url'] = 'http://weixin.qq.com/download';	
		$template['topcolor'] = $color;
		if($template_type == "login") {
			$login_data['first']['value'] = t('!username login',array('!username'=>$user->name));
			$login_data['keyword1']['value'] = format_date(time(),'short');
			$login_data['keyword2']['value'] = 'IOS';
			$login_data['keyword3']['value'] = ip_address();
			$login_data['remark']['value'] = 'Please warning if not your login';
			$template['data'] = $login_data;
		} else {
			$template['data'] = $data;
		}
	//	dpm($template);
	//	dpm(json_encode($template));	
		weixin_template_message($template);
		rules_log('Pepole view the user !name', array('!name' => $user->name));
	}
}
