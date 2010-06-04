SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';


-- -----------------------------------------------------
-- Table `users`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `users` ;

CREATE  TABLE IF NOT EXISTS `users` (
  `id` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `username` VARCHAR(64) NOT NULL ,
  `passwd` VARCHAR(40) NOT NULL ,
  `mail` VARCHAR(64) NULL ,
  `description` TEXT NULL,
  `rules_clients` TEXT NULL ,
  `rules_subnets` TEXT NULL ,
  `default_clients` ENUM('DROP','ACCEPT') NOT NULL DEFAULT 'DROP' ,
  `default_subnets` ENUM('DROP','ACCEPT') NOT NULL DEFAULT 'DROP' ,
  `start_time` TIMESTAMP NULL ,
  `end_time` TIMESTAMP NULL ,
  PRIMARY KEY (`id`) ,
  UNIQUE INDEX `id_UNIQUE` (`id` ASC) ,
  UNIQUE INDEX `username_UNIQUE` (`username` ASC) )
ENGINE = InnoDB;

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
    REFERENCES `users` (`id` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB
COMMENT = 'History of connections from clients';
