angular.module('be.gauge', [])
	.directive('beGauge', function() {
		return {
			'scope': { 
				'value': '@',
				'max': '@',
				'guide': '@',
				'size': '@',
				'width': '@strokeWidth'
			},
			'replace': true,
			'transclude': true,
			'template': '<svg width="{{size}}" height="{{size}}">' + 
							'<circle cx="{{size/2}}" cy="{{size/2}}" r="{{size/2-width}}" fill="none" stroke="#eee" />' + 
							'<path d="M{{size/2}},{{width}} A{{size/2-width}},{{size/2-width}} 0 {{overHalf}},1 {{endX}},{{endY}}" fill="none" />' + 
						'</svg>',
			'link': function(scope, element, attrs) {
				function update() {
					var angle = ((attrs.max - Math.max(attrs.value, 0)) / attrs.max) * 2 * Math.PI - .5 * Math.PI;
					
					scope.endX = (attrs.size/2) + (attrs.size/2 - attrs.strokeWidth) * Math.cos(angle);
					scope.endY = (attrs.size/2) + (attrs.size/2 - attrs.strokeWidth) * Math.sin(angle);
					
					scope.overHalf = angle > Math.PI * .5 ? '1' : '0';
				}
				attrs.$observe('value', update);
				attrs.$observe('max', update);
			}
		}
	});