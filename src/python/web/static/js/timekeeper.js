angular.module('be.timekeeper', [])
	.factory('Timekeeper', function($timeout, config) {
		return {	
			attach: function(container, timezone) {
				if (timezone !== 'local') {
					throw new Error("Timekeeper currently only supports the 'local' keyword for a timezone");
				}
				
				this.dataContainers = this.dataContainers || [];
				
				// Can't .push() the container, because it breaks the reference
				this.dataContainers.push({
					timezone: timezone
				});
				this.dataContainers[this.dataContainers.length-1].container = container;
				
				container = new Date(2342341);
				
				this.start();
				
				return this;
			},
			
			start: function() {
				this.keepTicking = true;
				this.tick();
			},
			
			tick: function() {
				console.log('teach')
				var that = this;
				angular.forEach(this.dataContainers, function(objCopy, key) {
					console.log(key);
					that.dataContainers[key].container.time = new Date();
					console.log('new date');
					// Preserve `that` within tick
					if (that.keepTicking) $timeout(function(){that.tick.call(that)}, 6e4 - Date.now() % 6e4 + 50 );
					console.log(6e4 - Date.now() % 6e4 + 50 );
				});
			}
		}
	});