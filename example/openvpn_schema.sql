SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';


-- -----------------------------------------------------
-- Table `users`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `users` ;

CREATE  TABLE IF NOT EXISTS `users` (
  `user_id` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `user_name` VARCHAR(64) NOT NULL ,
  `user_passwd` VARCHAR(40) NOT NULL ,
  `user_mail` VARCHAR(64) NULL ,
  PRIMARY KEY (`user_id`) ,
  UNIQUE INDEX `id_UNIQUE` (`user_id` ASC) ,
  UNIQUE INDEX `username_UNIQUE` (`user_name` ASC) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `groups`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `groups` ;

CREATE  TABLE IF NOT EXISTS `groups` (
  `group_id` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `group_name` VARCHAR(45) NOT NULL ,
  `group_description` TEXT NULL ,
  PRIMARY KEY (`group_id`) ,
  UNIQUE INDEX `id_UNIQUE` (`group_id` ASC) ,
  UNIQUE INDEX `groupname_UNIQUE` (`group_name` ASC) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `group_users`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `group_users` ;

CREATE  TABLE IF NOT EXISTS `group_users` (
  `group_id` INT UNSIGNED NOT NULL ,
  `user_id` INT UNSIGNED NOT NULL ,
  UNIQUE INDEX `group_user` (`group_id` ASC, `user_id` ASC) ,
  INDEX `fk_group_users_group_id` (`group_id` ASC) ,
  INDEX `fk_group_users_user_id` (`user_id` ASC) ,
  CONSTRAINT `fk_group_users_group_id`
    FOREIGN KEY (`group_id` )
    REFERENCES `groups` (`group_id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_group_users_user_id`
    FOREIGN KEY (`user_id` )
    REFERENCES `users` (`user_id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `connection_history`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `connection_history` ;

CREATE  TABLE IF NOT EXISTS `connection_history` (
  `id` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `user_id` INT UNSIGNED NOT NULL ,
  `start_time` TIMESTAMP NULL COMMENT 'time at which client connected' ,
  `end_time` TIMESTAMP NULL COMMENT 'time at which client disconnected =\ntime_unix + time_duration' ,
  `bytes_received` INT UNSIGNED NULL COMMENT 'Total number of bytes received from client during VPN session' ,
  `bytes_sent` INT UNSIGNED NULL COMMENT 'Total number of bytes sent to client during VPN session' ,
  `trusted_ip` INT UNSIGNED NULL COMMENT 'ip the client connected from' ,
  `trusted_port` SMALLINT UNSIGNED NULL COMMENT 'port the client connected from' ,
  `ifconfig_pool_remote_ip` INT UNSIGNED NULL COMMENT 'ip given to the client' ,
  PRIMARY KEY (`id`) ,
  INDEX `user_id_idx` (`user_id` ASC) ,
  CONSTRAINT `fk_connection_history_user_id`
    FOREIGN KEY (`user_id` )
    REFERENCES `users` (`user_id` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB
COMMENT = 'History of connections from clients';


-- -----------------------------------------------------
-- Table `access_permissions`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `access_permissions` ;

CREATE  TABLE IF NOT EXISTS `access_permissions` (
  `access_permission_id` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `start_time` TIMESTAMP NULL ,
  `end_time` TIMESTAMP NULL ,
  PRIMARY KEY (`access_permission_id`) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `firewall_rules`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `firewall_rules` ;

CREATE  TABLE IF NOT EXISTS `firewall_rules` (
  `firewall_rule_id` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `name` VARCHAR(45) NULL ,
  `description` TEXT NULL ,
  `rules_clients` TEXT NULL ,
  `rules_subnets` TEXT NULL ,
  `default_clients` ENUM('DROP','ACCEPT') NOT NULL DEFAULT 'DROP' ,
  `default_subnets` ENUM('DROP','ACCEPT') NOT NULL DEFAULT 'DROP' ,
  PRIMARY KEY (`firewall_rule_id`) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `user_access_permissions`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `user_access_permissions` ;

CREATE  TABLE IF NOT EXISTS `user_access_permissions` (
  `user_id` INT UNSIGNED NOT NULL ,
  `access_permission_id` INT UNSIGNED NOT NULL ,
  UNIQUE INDEX `user_access_permission` (`user_id` ASC, `access_permission_id` ASC) ,
  INDEX `fk_user_access_permissions_user_id` (`user_id` ASC) ,
  INDEX `fk_user_access_permissions_access_permission_id` (`access_permission_id` ASC) ,
  CONSTRAINT `fk_user_access_permissions_user_id`
    FOREIGN KEY (`user_id` )
    REFERENCES `users` (`user_id` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_user_access_permissions_access_permission_id`
    FOREIGN KEY (`access_permission_id` )
    REFERENCES `access_permissions` (`access_permission_id` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `user_firewall_rules`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `user_firewall_rules` ;

CREATE  TABLE IF NOT EXISTS `user_firewall_rules` (
  `user_id` INT UNSIGNED NOT NULL ,
  `firewall_rule_id` INT UNSIGNED NOT NULL ,
  UNIQUE INDEX `user_firewall_rules` (`user_id` ASC, `firewall_rule_id` ASC) ,
  INDEX `fk_user_firewall_rules_user_id` (`user_id` ASC) ,
  INDEX `fk_user_firewall_rules_firewall_rule_id` (`firewall_rule_id` ASC) ,
  CONSTRAINT `fk_user_firewall_rules_user_id`
    FOREIGN KEY (`user_id` )
    REFERENCES `users` (`user_id` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_user_firewall_rules_firewall_rule_id`
    FOREIGN KEY (`firewall_rule_id` )
    REFERENCES `firewall_rules` (`firewall_rule_id` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `group_firewall_rules`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `group_firewall_rules` ;

CREATE  TABLE IF NOT EXISTS `group_firewall_rules` (
  `group_id` INT UNSIGNED NOT NULL ,
  `firewall_rule_id` INT UNSIGNED NOT NULL ,
  UNIQUE INDEX `group_firewall` (`group_id` ASC, `firewall_rule_id` ASC) ,
  INDEX `fk_group_firewall_rules_group_id` (`group_id` ASC) ,
  INDEX `fk_group_firewall_rules_firewall_rule_id` (`firewall_rule_id` ASC) ,
  CONSTRAINT `fk_group_firewall_rules_group_id`
    FOREIGN KEY (`group_id` )
    REFERENCES `groups` (`group_id` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_group_firewall_rules_firewall_rule_id`
    FOREIGN KEY (`firewall_rule_id` )
    REFERENCES `firewall_rules` (`firewall_rule_id` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `group_access_permissions`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `group_access_permissions` ;

CREATE  TABLE IF NOT EXISTS `group_access_permissions` (
  `group_id` INT UNSIGNED NOT NULL ,
  `access_permission_id` INT UNSIGNED NOT NULL ,
  INDEX `fk_group_access_permissions_group_id` (`group_id` ASC) ,
  INDEX `fk_group_access_permissions_access_permission_id` (`access_permission_id` ASC) ,
  UNIQUE INDEX `group_access_uniq` (`group_id` ASC, `access_permission_id` ASC) ,
  CONSTRAINT `fk_group_access_permissions_group_id`
    FOREIGN KEY (`group_id` )
    REFERENCES `groups` (`group_id` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_group_access_permissions_access_permission_id`
    FOREIGN KEY (`access_permission_id` )
    REFERENCES `access_permissions` (`access_permission_id` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB;



SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
