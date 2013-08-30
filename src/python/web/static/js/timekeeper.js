angular.module('be.timekeeper', [])
	.factory('Timekeeper', function($timeout, config) {
		return {	
			attach: function(container, timezone) {
				
				this.dataContainers = this.dataContainers || [];
				
				this.dataContainers.push({
					timezone: timezone
				});
				// Can't .push() the container, because it breaks the reference
				this.dataContainers[this.dataContainers.length-1].container = container;
				
				container = this.zonedTime(timezone);
				
				this.start();
				
				return this;
			},
			
			start: function() {
				this.keepTicking = true;
				this.tick();
			},
			
			tick: function() {
				var that = this;
				angular.forEach(this.dataContainers, function(objCopy, key) {
					that.dataContainers[key].container.time = that.zonedTime( objCopy.timezone );
					
					// Preserve `that` within tick
					if (that.keepTicking) $timeout(function(){that.tick.call(that)}, 6e4 - Date.now() % 6e4 + 50 );
				});
			}, 
			
			zonedTime: function(zone) {
				var offset = (zone === "local") ? 0 : parseInt(zone, 10);
				
				var date = new Date();
				date.setHours(date.getHours() + offset);
				return date;
			}
		}
	});