<?php
/*
$Id$

  This code is part of LDAP Account Manager (http://www.ldap-account-manager.org/)
  Copyright (C) 2011  Roland Gruber

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/**
* Manages FreeRadius accounts.
*
* @package modules
* @author Roland Gruber
*/

/**
* Manages FreeRadius accounts.
*
* @package modules
*/
class freeRadius extends baseModule {

	/**
	* Creates a new freeRadius object.
	*
	* @param string $scope account type (user, group, host)
	*/
	function __construct($scope) {
		parent::__construct($scope);
		$this->autoAddObjectClasses = false;
	}

	/**
	* Returns meta data that is interpreted by parent class
	*
	* @return array array with meta data
	* 
	* @see baseModule::get_metaData()
	*/
	function get_metaData() {
		$return = array();
		// icon
		$return['icon'] = 'freeRadius.png';
		// manages user accounts
		$return["account_types"] = array("user");
		// alias name
		$return["alias"] = _("FreeRadius");
		// module dependencies
		$return['dependencies'] = array('depends' => array(array('posixAccount', 'inetOrgPerson')), 'conflicts' => array());
		// managed object classes
		$return['objectClasses'] = array('radiusprofile');
		// managed attributes
		$return['attributes'] = array('radiusFramedIPAddress', 'radiusFramedIPNetmask', 'radiusRealm');
		// help Entries
		$return['help'] = array(
			'radiusFramedIPAddress' => array(
				"Headline" => _("IP address"),
				"Text" => _("This is the IP address for the user (e.g. 123.123.123.123).")
			),
			'radiusFramedIPNetmask' => array(
				"Headline" => _("Net mask"),
				"Text" => _("The net mask for the IP address.")
			),
			'radiusRealm' => array(
				"Headline" => _("Realm"),
				"Text" => _("The Radius realm of this account.")
			),
			'hiddenOptions' => array(
				"Headline" => _("Hidden options"),
				"Text" => _("The selected options will not be managed inside LAM. You can use this to reduce the number of displayed input fields.")
			));
		// configuration settings
		$configContainer = new htmlTable();
		$configContainerHead = new htmlTable();
		$configContainerHead->addElement(new htmlOutputText(_('Hidden options')));
		$configContainerHead->addElement(new htmlHelpLink('hiddenOptions'));
		$configContainerOptions = new htmlTable();
		$configContainer->addElement($configContainerHead, true);
		$configContainerOptions->addElement(new htmlTableExtendedInputCheckbox('freeRadius_hideRadiusFramedIPAddress', false, _('IP address'), null, false));
		$configContainerOptions->addElement(new htmlOutputText(' '));
		$configContainerOptions->addElement(new htmlTableExtendedInputCheckbox('freeRadius_hideRadiusFramedIPNetmask', false, _('Net mask'), null, false));
		$configContainerOptions->addElement(new htmlOutputText(' '));
		$configContainerOptions->addElement(new htmlTableExtendedInputCheckbox('freeRadius_hideRadiusRealm', false, _('Realm'), null, false));
		$configContainer->addElement($configContainerOptions, true);
		$return['config_options']['all'] = $configContainer;
		// profile settings
		$profileElements = array();
		if (!$this->isBooleanConfigOptionSet('freeRadius_hideRadiusFramedIPNetmask')) {
			$profileElements[] = new htmlTableExtendedInputField(_('Net mask'), 'freeRadius_radiusFramedIPNetmask', null, 'radiusFramedIPNetmask');
			$return['profile_checks']['freeRadius_radiusFramedIPNetmask'] = array(
				'type' => 'ext_preg',
				'regex' => 'ip',
				'error_message' => $this->messages['radiusFramedIPNetmask'][0]);
			$return['profile_mappings']['freeRadius_radiusFramedIPNetmask'] = 'radiusFramedIPNetmask';
		}
		if (!$this->isBooleanConfigOptionSet('freeRadius_hideRadiusRealm')) {
			$profileElements[] = new htmlTableExtendedInputField(_('Realm'), 'freeRadius_radiusRealm', null, 'radiusRealm');
			$return['profile_checks']['freeRadius_radiusRealm'] = array(
				'type' => 'ext_preg',
				'regex' => 'DNSname',
				'error_message' => $this->messages['radiusRealm'][0]);
			$return['profile_mappings']['freeRadius_radiusRealm'] = 'radiusRealm';
		}
		if (sizeof($profileElements) > 0) {
			$profileContainer = new htmlTable();
			for ($i = 0; $i < sizeof($profileElements); $i++) {
				$profileContainer->addElement($profileElements[$i]);
				$profileContainer->addNewLine();
			}
			$return['profile_options'] = $profileContainer;
		}
		// upload fields
		$return['upload_columns'] = array();
		if (!$this->isBooleanConfigOptionSet('freeRadius_hideRadiusFramedIPAddress')) {
			$return['upload_columns'][] = array(
				'name' => 'freeRadius_radiusFramedIPAddress',
				'description' => _('IP address'),
				'help' => 'radiusFramedIPAddress',
				'example' => '123.123.123.123',
			);
		}
		if (!$this->isBooleanConfigOptionSet('freeRadius_hideRadiusFramedIPNetmask')) {
			$return['upload_columns'][] = array(
				'name' => 'freeRadius_radiusFramedIPNetmask',
				'description' => _('Net mask'),
				'help' => 'radiusFramedIPNetmask',
				'example' => '255.255.255.0'
			);
		}
		if (!$this->isBooleanConfigOptionSet('freeRadius_hideRadiusRealm')) {
			$return['upload_columns'][] = array(
				'name' => 'freeRadius_radiusRealm',
				'description' => _('Realm'),
				'help' => 'radiusRealm',
				'example' => _('company.com')
			);
		}
		// available PDF fields
		$return['PDF_fields'] = array();
		if (!$this->isBooleanConfigOptionSet('freeRadius_hideRadiusFramedIPAddress')) {
			$return['PDF_fields']['radiusFramedIPAddress'] = _('IP address');
		}
		if (!$this->isBooleanConfigOptionSet('freeRadius_hideRadiusFramedIPNetmask')) {
			$return['PDF_fields']['radiusFramedIPNetmask'] = _('Net mask');
		}
		if (!$this->isBooleanConfigOptionSet('freeRadius_hideRadiusRealm')) {
			$return['PDF_fields']['radiusRealm'] = _('Realm');
		}
		return $return;
	}

	/**
	* This function fills the error message array with messages
	*/
	function load_Messages() {
		$this->messages['radiusFramedIPAddress'][0] = array('ERROR', _('The IP address is invalid.'));
		$this->messages['radiusFramedIPAddress'][1] = array('ERROR', _('Account %s:') . ' freeRadius_radiusFramedIPAddress', _('The IP address is invalid.'));
		$this->messages['radiusFramedIPNetmask'][0] = array('ERROR', _('The net mask is invalid.'));
		$this->messages['radiusFramedIPNetmask'][1] = array('ERROR', _('Account %s:') . ' freeRadius_radiusFramedIPNetmask', _('The net mask is invalid.'));
		$this->messages['radiusRealm'][0] = array('ERROR', _('Please enter a valid realm.'));
		$this->messages['radiusRealm'][1] = array('ERROR', _('Account %s:') . ' freeRadius_radiusRealm', _('Please enter a valid realm.'));
	}

	/**
	 * Returns the HTML meta data for the main account page.
	 * 
	 * @return htmlElement HTML meta data
	 */
	function display_html_attributes() {
		$return = new htmlTable();
		if (in_array('radiusprofile', $this->attributes['objectClass'])) {
			// IP address
			if (!$this->isBooleanConfigOptionSet('freeRadius_hideRadiusFramedIPAddress')) {
				$radiusFramedIPAddress = '';
				if (isset($this->attributes['radiusFramedIPAddress'][0])) {
					$radiusFramedIPAddress = $this->attributes['radiusFramedIPAddress'][0];
				}
				$return->addElement(new htmlTableExtendedInputField(_('IP address'), 'radiusFramedIPAddress', $radiusFramedIPAddress, 'radiusFramedIPAddress'), true);
			}
			// net mask
			if (!$this->isBooleanConfigOptionSet('freeRadius_hideRadiusFramedIPNetmask')) {
				$radiusFramedIPNetmask = '';
				if (isset($this->attributes['radiusFramedIPNetmask'][0])) {
					$radiusFramedIPNetmask = $this->attributes['radiusFramedIPNetmask'][0];
				}
				$return->addElement(new htmlTableExtendedInputField(_('Net mask'), 'radiusFramedIPNetmask', $radiusFramedIPNetmask, 'radiusFramedIPNetmask'), true);
			}
			// realm
			if (!$this->isBooleanConfigOptionSet('freeRadius_hideRadiusRealm')) {
				$radiusRealm = '';
				if (isset($this->attributes['radiusRealm'][0])) {
					$radiusRealm = $this->attributes['radiusRealm'][0];
				}
				$return->addElement(new htmlTableExtendedInputField(_('Realm'), 'radiusRealm', $radiusRealm, 'radiusRealm'), true);
			}
			// button to remove extension
			$return->addElement(new htmlSpacer(null, '10px'), true);
			$remButton = new htmlButton('remObjectClass', _('Remove FreeRadius extension'));
			$remButton->colspan = 3;
			$return->addElement($remButton);
		}
		else {
			$return->addElement(new htmlButton('addObjectClass', _('Add FreeRadius extension')));
		}
		return $return;
	}

	/**
	* Processes user input of the primary module page.
	* It checks if all input values are correct and updates the associated LDAP attributes.
	*
	* @return array list of info/error messages
	*/
	function process_attributes() {
		if (isset($_POST['addObjectClass'])) {
			$this->attributes['objectClass'][] = 'radiusprofile';
			return array();
		}
		elseif (isset($_POST['remObjectClass'])) {
			$this->attributes['objectClass'] = array_delete(array('radiusprofile'), $this->attributes['objectClass']);
			for ($i = 0; $i < sizeof($this->meta['attributes']); $i++) {
				if (isset($this->attributes[$this->meta['attributes'][$i]])) {
					unset($this->attributes[$this->meta['attributes'][$i]]);
				}
			}
			return array();
		}
		// skip processing if extension is not active
		if (!in_array('radiusprofile', $this->attributes['objectClass'])) {
			return array();
		}
		$errors = array();
		// IP address
		if (!$this->isBooleanConfigOptionSet('freeRadius_hideRadiusFramedIPAddress')) {
			$this->attributes['radiusFramedIPAddress'][0] = $_POST['radiusFramedIPAddress'];
			if (($_POST['radiusFramedIPAddress'] != '') && !get_preg($_POST['radiusFramedIPAddress'], 'ip')) {
				$errors[] = $this->messages['radiusFramedIPAddress'][0];
			}
		}
		// net mask
		if (!$this->isBooleanConfigOptionSet('freeRadius_hideRadiusFramedIPNetmask')) {
			$this->attributes['radiusFramedIPNetmask'][0] = $_POST['radiusFramedIPNetmask'];
			if (($_POST['radiusFramedIPNetmask'] != '') && !get_preg($_POST['radiusFramedIPNetmask'], 'ip')) {
				$errors[] = $this->messages['radiusFramedIPNetmask'][0];
			}
		}
		// realm
		if (!$this->isBooleanConfigOptionSet('freeRadius_hideRadiusRealm')) {
			$this->attributes['radiusRealm'][0] = $_POST['radiusRealm'];
			if (($_POST['radiusRealm'] != '') && !get_preg($_POST['radiusRealm'], 'DNSname')) {
				$errors[] = $this->messages['radiusRealm'][0];
			}
		}
		return $errors;
	}

	/**
	* In this function the LDAP account is built up.
	*
	* @param array $rawAccounts list of hash arrays (name => value) from user input
	* @param array $partialAccounts list of hash arrays (name => value) which are later added to LDAP
	* @param array $ids list of IDs for column position (e.g. "posixAccount_uid" => 5)
	* @param array $selectedModules list of selected account modules
	* @return array list of error messages if any
	*/
	function build_uploadAccounts($rawAccounts, $ids, &$partialAccounts, $selectedModules) {
		$errors = array();
		for ($i = 0; $i < sizeof($rawAccounts); $i++) {
			// add object class
			if (!in_array("radiusprofile", $partialAccounts[$i]['objectClass'])) $partialAccounts[$i]['objectClass'][] = "radiusprofile";
			// IP address
			if ($rawAccounts[$i][$ids['freeRadius_radiusFramedIPAddress']] != "") {
				if (get_preg($rawAccounts[$i][$ids['freeRadius_radiusFramedIPAddress']], 'ip')) {
					$partialAccounts[$i]['radiusFramedIPAddress'] = $rawAccounts[$i][$ids['freeRadius_radiusFramedIPAddress']];
				}
				else {
					$errMsg = $this->messages['radiusFramedIPAddress'][1];
					array_push($errMsg, array($i));
					$errors[] = $errMsg;
				}
			}
			// net mask
			if ($rawAccounts[$i][$ids['freeRadius_radiusFramedIPNetmask']] != "") {
				if (get_preg($rawAccounts[$i][$ids['freeRadius_radiusFramedIPNetmask']], 'ip')) {
					$partialAccounts[$i]['radiusFramedIPNetmask'] = $rawAccounts[$i][$ids['freeRadius_radiusFramedIPNetmask']];
				}
				else {
					$errMsg = $this->messages['radiusFramedIPNetmask'][1];
					array_push($errMsg, array($i));
					$errors[] = $errMsg;
				}
			}
			// realm
			if ($rawAccounts[$i][$ids['freeRadius_radiusRealm']] != "") {
				if (get_preg($rawAccounts[$i][$ids['freeRadius_radiusRealm']], 'DNSname')) {
					$partialAccounts[$i]['radiusRealm'] = $rawAccounts[$i][$ids['freeRadius_radiusRealm']];
				}
				else {
					$errMsg = $this->messages['radiusRealm'][1];
					array_push($errMsg, array($i));
					$errors[] = $errMsg;
				}
			}
		}
		return $errors;
	}

	/**
	* Returns the PDF entries for this module.
	*
	* @return array list of possible PDF entries
	*/
	function get_pdfEntries() {
		$return = array();
		if (isset($this->attributes['radiusFramedIPAddress'][0])) {
			$return[get_class($this) . '_radiusFramedIPAddress'][0] = '<block><key>' . _('IP address') . '</key><value>' . $this->attributes['radiusFramedIPAddress'][0] . '</value></block>';
		}
		if (isset($this->attributes['radiusFramedIPNetmask'][0])) {
			$return[get_class($this) . '_radiusFramedIPNetmask'][0] = '<block><key>' . _('Net mask') . '</key><value>' . $this->attributes['radiusFramedIPNetmask'][0] . '</value></block>';
		}
		if (isset($this->attributes['radiusRealm'][0])) {
			$return[get_class($this) . '_radiusRealm'][0] = '<block><key>' . _('Realm') . '</key><value>' . $this->attributes['radiusRealm'][0] . '</value></block>';
		}
		return $return;
	}

}


?>