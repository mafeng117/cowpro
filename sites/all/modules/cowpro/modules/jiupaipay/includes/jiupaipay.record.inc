<?php
include_once 'jiupaipay.const.inc';
include_once ("libs/RSA.php");
include_once ("libs/encrypt.php");
include_once ("libs/AES.php");
include_once ("libs/Common.php");

/**
 * 交易状态查询（C00001）
 *
 * @param int $uid
 */
function _jiupaipay_cowpro_fund_query_record($uid) {
	// 请求报文头
	$header ['version'] = '1.0';
	$header ['merchantCode'] = 'jbz';
	$header ['txnType'] = 'C00001';
	$header ['clientSn'] = $header ['txnType'] . "@" . round ( microtime ( true ) * 1000 );
	$header ['clientDate'] = date ( "Ymd", time () );
	$header ['clientTime'] = date ( "His", time () );
	$header ['fileName'] = "";
	$header ['signTime'] = round ( microtime ( true ) * 1000 );

	$aes = new AES ( JPP_AES_KEY, JPP_AES_IV );
	$post_data = array ();
	$post_data ['businessSeqNo'] = "jbz" . date ( "Ymd", time () ) . time ();
	$post_data ['oldbusinessSeqNo'] = $_POST ['record_No'];
	$post_data ['operType'] = $_POST ['record_types'];
	// var_dump($post_data);

	ksort ( $post_data );
	$signInfo = $header ['signTime'];
	foreach ( $post_data as $value ) {
		$signInfo .= "|" . $value;
	}
	$rsa_private_key = variable_get ( PRIVATE_KEY_ON_JIUPAIPAY );
	$rsa_public_key = variable_get ( PUBLIC_KEY_ON_JIUPAIPAY );
	$rsa = new RSA ( $rsa_private_key, $rsa_public_key );
	$signData = $rsa->sign ( $signInfo );

	$header ['signature'] = $signData;
	$curl_post_data ['reqHeader'] = $header;
	$curl_post_data ['inBody'] = $post_data;

	$ch = curl_init ( JPP_URL );
	curl_setopt ( $ch, CURLOPT_URL, JPP_URL );
	curl_setopt ( $ch, CURLOPT_HEADER, 0 );
	curl_setopt ( $ch, CURLOPT_POST, 1 );
	curl_setopt ( $ch, CURLOPT_POSTFIELDS, json_encode ( $curl_post_data ) );
	curl_setopt ( $ch, CURLOPT_RETURNTRANSFER, 1 );
	curl_setopt ( $ch, CURLOPT_SSL_VERIFYPEER, false );
	curl_setopt ( $ch, CURLOPT_HTTPHEADER, array (
			'Content-Type: application/json',
			'Content-Length: ' . strlen ( json_encode ( $curl_post_data ) )
	) );

	$response = curl_exec ( $ch );
	if (curl_errno ( $ch )) {
		echo 'Errno ' . curl_error ( $ch ); // 捕抓异常
	}
	curl_close ( $ch );
	$response = json_decode ( $response, true );
	// var_dump($response);die();

	$result = array ();
	if ($response ['respHeader'] ['respCode'] == 'P2P0000') {
		$result ['oldbusinessSeqNo'] = $response ['outBody'] ['oldbusinessSeqNo'];
		$result ['dealStatus'] = $response ['outBody'] ['dealStatus'];
		$result ['msg'] = $response ['outBody'] ['respMsg'];
	} else {
		$result ['oldbusinessSeqNo'] = '';
		$result ['dealStatus'] = 0;
		$result ['msg'] = $response ['respHeader'] ['respMsg'];
	}
	return $result;
}