<?php

/*

  This code is part of LDAP Account Manager (http://www.sourceforge.net/projects/lam)
  Copyright (C) 2009 - 2012  Pozdnyak Pavel
                2010 - 2024  Roland Gruber

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
 * The account type for Asterisk extensions.
 *
 * @package types
 * @author Pozdnyak Pavel
 * @author Roland Gruber
 */

/**
 * The account type for Asterisk extensions.
 *
 * @package types
 */
class asteriskExt extends baseType {

	/**
	 * Constructs a new domain type object.
	 *
	 * @param ConfiguredType $type configuration
	 */
	public function __construct($type) {
		parent::__construct($type);
		$this->LABEL_CREATE_ANOTHER_ACCOUNT = _('Create another extension');
		$this->LABEL_BACK_TO_ACCOUNT_LIST = _('Back to extensions list');
	}

	/**
	 * Returns the alias name of this account type.
	 *
	 * @return string alias name
	 */
	function getAlias() {
		return _("Asterisk extensions");
	}

	/**
	 * Returns the description of this account type.
	 *
	 * @return string description
	 */
	function getDescription() {
		return _("Asterisk extensions entries");
	}

	/**
	 * Returns the class name for the list object.
	 *
	 * @return string class name
	 */
	function getListClassName() {
		return "lamAsteriskExtList";
	}

	/**
	 * Returns the default attribute list for this account type.
	 *
	 * @return string attribute list
	 */
	function getDefaultListAttributes() {
		return "#AstExtension;#AstContext;#member";
	}

	/**
	 * Returns the the title text for the title bar on the new/edit page.
	 *
	 * @param accountContainer $container account container
	 * @return String title text
	 */
	public function getTitleBarTitle($container) {
		$attributes = null;
		if ($container->getAccountModule('asteriskExtension') != null) {
			$attributes = $container->getAccountModule('asteriskExtension')->getAttributes();
		}
		// check if a common name is set
		if (isset($attributes['AstExtension'][0])) {
			return htmlspecialchars($attributes['AstExtension'][0]);
		}
		// new account
		if ($container->isNewAccount) {
			return _("New extension");
		}
		// fall back to default
		return parent::getTitleBarTitle($container);
	}

}

/**
 * Generates the list view.
 *
 * @package lists
 * @author Pozdnyak Pavel
 *
 */
class lamAsteriskExtList extends lamList {

	/**
	 * Constructor
	 *
	 * @param string $type account type
	 * @return lamList list object
	 */
	function __construct($type) {
		parent::__construct($type);
		$this->labels = [
			'nav' => _("Extension count: %s"),
			'error_noneFound' => _("No Asterisk extensions found."),
			'newEntry' => _("New extension"),
			'deleteEntry' => _("Delete selected extensions")
		];
	}

	/**
	 * Forces the list view to show only specific attributes.
	 *
	 * @see lamList::listGetParams()
	 */
	protected function listGetParams() {
		// check if only PDF should be shown
		parent::listGetParams();
		$this->attrArray = ["astextension", "astcontext", "member"];
		$this->descArray = [_("Extension name"), _("Account context"), _("Owner")];
	}

	/**
	 * Groups the extensions.
	 *
	 * (non-PHPdoc)
	 * @see lamList::listRefreshData()
	 */
	protected function listRefreshData() {
		// configure search filter
		$module_filter = get_ldap_filter($this->type->getId());  // basic filter is provided by modules
		$filter = "(&" . $module_filter . $this->buildLDAPAttributeFilter() . ")";
		$attrs = $this->attrArray;
		$attrs[] = "astpriority";
		$entries = searchLDAP($this->suffix, $filter, $attrs);
		$lastError = getLastLDAPError();
		if ($lastError != null) {
			call_user_func_array('StatusMessage', $lastError);
		}

		$this->ldapEntries = $this->normalizeLdapOutput($entries);
		$this->entries = [];
		foreach ($this->ldapEntries as $index => &$attrs) {
			$this->entries[$index] = &$attrs;
		}
		// generate list of possible suffixes
		$this->possibleSuffixes = $this->type->getSuffixList();
	}

	/**
	 * Groups the extensions.
	 *
	 * @param array $entries extension entries
	 */
	private function normalizeLdapOutput($entries) {
		$filteredEntries = [];
		foreach ($entries as $entry) {
			if ($entry["astpriority"][0] == '1') {
				$filteredEntries[] = $entry;
			}
		}
		return $filteredEntries;
	}

}
