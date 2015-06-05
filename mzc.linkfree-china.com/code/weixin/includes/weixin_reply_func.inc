<?php

function weixin_nodes_reply($nodes, $wechat_obj) {
//		$this->set_response('advanced');
	if (!empty($nodes)) {
		$count = 0;
		foreach ($nodes as $node) {
			file_put_contents('/tmp/node',var_export($node, true));
			$title = $node->title;
			$body = drupal_substr(strip_tags($node->body[$node->language][0]['safe_value']),0,10);
			if(!empty($node->field_image)) {
				$thumb = file_create_url($node->field_image[$node->language][0]['uri']);
			} else {
				$thumb = '';
			}
			$url = url('node/'.$node->nid, array('absolute' => TRUE));
			$items = $items . $wechat_obj->get_item($title, $excerpt, $thumb, $url);
			$count++;
		}
		echo sprintf($wechat_obj->get_picTpl(), $count, $items);
		file_put_contents('/tmp/news',var_export($news, true));
	}
}

function weixin_new_posts_reply($wechat_obj) {
		$nodes = node_get_recent(5);
		weixin_nodes_reply($nodes, $wechat_obj);
		file_put_contents('/tmp/reply_function',var_export($nodes, true));
}

function weixin_custom_nodes_reply($wechat_obj, $keyword) {
		$nodes = array();
		$node_list = split(",", $keyword);
//		file_put_contents('/tmp/reply_nodes',var_export($nodes, true));
		foreach ($node_list as $node_id) {
			$nodes[$node_id]=node_load($node_id);
		}
		weixin_nodes_reply($nodes, $wechat_obj);
}

function weixin_term_posts_reply($wechat_obj, $keyword) {
		$nodes = array();
		$node_list = taxonomy_select_nodes($keyword, $pager = TRUE, $limit = 5);
		foreach ($node_list as $node_id) {
//			file_put_contents('/tmp/node_id',var_export($node_id, true));
			$nodes[$node_id]=node_load($node_id);
		}
		weixin_nodes_reply($nodes, $wechat_obj);
//		file_put_contents('/tmp/term_nodes',var_export($nodes, true));
}
