angular.module('be.poller', [])
	.factory('Poller', function($http, $timeout, config) {
		return {	
			keepTicking: false,
			tick: function() {
				var that = this;
				
				if (angular.isFunction(that.sendDataFn)) sendData = that.sendDataFn();
				// Error has to be first because reasons
				$http.get('update', {params: sendData}).error(function(r) {
					if (!angular.isFunction(that.onErrorFn) || that.onErrorFn(r) !== false) {
						// Keep on ticking unless onErrorFn exists and returned false
						if (that.keepTicking) $timeout(function() {that.tick.call(that)}, config.pollInterval);
					}
				}).then(function(r) {
					var data = (angular.isFunction(that.interceptFn)) ? that.interceptFn(r.data) : r.data;
					
					if (that.dataContainer) _.extend(that.dataContainer, data);
					
					// Preserve `this` within tick()
					if (that.keepTicking) $timeout(function() {that.tick.call(that)}, config.pollInterval);
					
					if (angular.isFunction(that.afterTickFn)) that.afterTickFn(that.dataContainer);
				});
				
				return this;
			},
			start: function(interval) {
				if (!angular.isNumber(interval) && !angular.isNumber(this.interval)) {
					throw Error("Interval is required and must be a number");
				} else if (angular.isNumber(interval)) {
					this.interval = interval;
				}
				this.keepTicking = true;
				this.tick();
				
				return this;
			},
			stop: function() {
				this.keepTicking = false;
				
				return this;
			},
			attach: function(thing) {
				this.dataContainer = thing;
				
				return this;
			},
			send: function(call) {
				this.sendDataFn = call;
				
				return this;
			},
			/**
			 * Transform the data. Callback is passed the data as it comes from the server. Whatever the callback
			 * returns is merged into the attached scope member. Make sure this function returns the transformed data.
			 */
			interceptor: function(call) {
				this.interceptFn = call;
				
				return this;
			},
			/**
			 * Post-tick-processing, such as detecting certain values in the returned data. This recieves the merged data,
			 * not only the data that's returned from intercept (or from the server if there is no intercept function).
			 */
			afterTick: function(call) {
				this.afterTickFn = call;
				
				return this;
			},
			onError: function(call) {
				this.onErrorFn = call;
				
				return this;
			}
		};
	});