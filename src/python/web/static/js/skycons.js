angular.module('skycons', [])
	.directive('skyconsIcon', function(config) {
		return {
			'scope': { 
				icon: '@skyconsIcon' // I don't think this is even used
			},
			'link': function(scope, element, attrs) {
				if (window.skycons == undefined) {
					window.skycons = new Skycons();
				}
				
				attrs.$observe('skyconsIcon', function(value) {
					var iconFnName =  value.toUpperCase().replace(/-/g, '_');
			        if (typeof Skycons[iconFnName] === 'function') {
			        	skycons.add(element[0], Skycons[iconFnName]);
			        	if (!config.isKindle) skycons.play();
			        } else if (iconFnName !== '') {
			        	console.warn("Skycon "+iconFnName+" was not found.");
			        }
			    });
			}
		}
	});
