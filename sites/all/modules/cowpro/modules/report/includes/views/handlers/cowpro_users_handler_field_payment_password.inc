<?php

class cowpro_users_handler_field_payment_password extends views_handler_field {
  function render($values) {
  	$result = '';
    $uid = $this->get_value($values);

    $profile = cowpro_customer_profile_load_one_row_with_conditions($uid, 'payment_password');
    if ($profile) {
    	$password = '***';//preg_replace('/(.)/', '*', $profile->password);
    } else {
    	$password = '';
    }
    return $password;
  }
}
